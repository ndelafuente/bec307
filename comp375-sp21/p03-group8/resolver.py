import sys
import socket
from struct import *
from random import randint

""" GLOBAL CONSTANTS """
# DNS Message Formats
# Special characters:
    # '!' = Network order
    # 'H' = unsigned short (2 bytes of data)
    # 'S' = variable length string
    # 'I' = unsigned integer (4 bytes of data)
    # 'B' = unsigned char (1 byte of data)
HEADER_FORMAT = 'HHHHHH'
QUERY_FORMAT = 'SHH' 
RECORD_FORMAT = 'SHHIH' # not including the actual resource data 
IP_FORMAT = 'BBBB'

# The values for the most common resource record types
(A, NS, CNAME, SOA, PTR, MX, TXT, AAAA) = (1, 2, 5, 6, 12, 15, 16, 28)

""" GLOBAL VARIABLES """
IP = 0
ROOT = ""

def stringToNetwork(orig_string):
    """
    Converts a standard string to a string that can be sent over
    the network.

    Args:
        orig_string (string): the string to convert

    Returns:
        bytes: The network formatted string (as bytes)

    Example:
        stringToNetwork('www.sandiego.edu.edu') will return
          (3)www(8)sandiego(3)edu(0)
    """
    ls = orig_string.split('.')
    toReturn = b""
    for item in ls:
        formatString = "B"
        formatString += str(len(item))
        formatString += "s"
        toReturn += pack(formatString, len(item), item.encode())
    toReturn += pack("B", 0)
    return toReturn


def networkToString(response, start):
    """
    Converts a network response string into a human readable string.

    Args:
        response (string): the entire network response message
        start (int): the location within the message where the network string
            starts.

    Returns:
        A (string, int) tuple
            - string: The human readable string.
            - int: The index one past the end of the string, i.e. the starting
              index of the value immediately after the string.

    Example:  networkToString('(3)www(8)sandiego(3)edu(0)', 0) would return
              ('www.sandiego.edu', 18)
    """

    toReturn = ""
    position = start
    length = -1
    while True:
        length = unpack("!B", response[position:position+1])[0]
        if length == 0:
            position += 1
            break

        # Handle DNS pointers (!!)
        elif (length & 1 << 7) and (length & 1 << 6):
            b2 = unpack("!B", response[position+1:position+2])[0]
            offset = 0
            """
            # strip off leading two bits shift by 8 to account for "length"
            # being the most significant byte
            ooffset += (length & 1 << i)ffset += (length & 0x3F) << 8  

            offset += b2
            """
            for i in range(6) :
                offset += (length & 1 << i) << 8
            for i in range(8):
                offset += (b2 & 1 << i)
            dereferenced = networkToString(response, offset)[0]
            return toReturn + dereferenced, position + 2

        formatString = str(length) + "s"
        position += 1
        toReturn += unpack(formatString, response[position:position+length])[0].decode()
        toReturn += "."
        position += length
    return toReturn[:-1], position


def networkToInteger(formatChar, response, start):
    """
    Converts a network response integer into a python native integer.

    Args:
        formatChar (string): a single character representing the size of the data.
        response (string): the entire network response message.
        start (int): the location within the message where the integer starts.

    Returns:
        A (int, int) tuple
            - int: The python native integer.
            - int: The index one past the end of the string, i.e. the starting
              index of the value immediately after the integer.
    """
    assert len(formatChar) == 1  # ensure the format char is a single character

    end = start + calcsize(formatChar) # calculate the end position of the int
    val = unpack('!' + formatChar, response[start: end])[0]
    
    return val, end


def constructQuery(ID, hostname, is_mx = False):
    """
    Constructs a DNS query message for a given hostname and ID.

    Args:
        ID (int): ID # for the message
        hostname (string): What we're asking for
        is_mx (boolean): True if constructing a query for an MX record, False if
          constructing a query for an A record.

    Returns:
        string: "Packed" string containing a valid DNS query message
    """
    flags = 0 # 0 implies basic iterative query

    # one question, no answers for basic query
    num_questions = 1
    num_answers = 0
    num_auth = 0
    num_other = 0

    # "!HHHHHH" means pack 6 Half integers (i.e. 16-bit values) into a single
    # string, with data placed in network order (!)
    header = pack("!HHHHHH", ID, flags, num_questions, num_answers, num_auth,
                  num_other)

    qname = stringToNetwork(hostname)
    qtype = 1 if not is_mx else 15 # request A type

    remainder = pack("!HH", qtype, 1)
    query = header + qname + remainder
    return query


def resolve(hostname, is_mx = False):
    """
    Returns a string with the IP address (for an A record) or name of mail
    server associated with the given hostname.

    Args:
        hostname (string): The name of the host to resolve.
        is_mx (boolean): True if requesting the MX record result, False if
          requesting the A record.

    Returns:
        string: A string representation of an IP address (e.g. "192.168.0.1") or
          mail server (e.g. "mail.google.com"). If the request could not be
          resolved, None will be returned.
    """

    # Declare global variables
    global IP   # the current ip being queried
    global ROOT # the original ip from one of the root DNS servers

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.settimeout(5)   # socket should timeout after 5 seconds

    # Create a query using a randomly generated ID
    id = randint(0, 2 ** 15 -1) 
    query = constructQuery(id, hostname, is_mx)

    try:
        # Select the IP to send the query to
        local_ip = selectIp()

        print("\nQuerying:", hostname, "using IP", local_ip)
        sock.sendto(query, (local_ip, 53)) # 53 is the well-known port number for DNS

        # Receive the response (timing out after 5 seconds)
        response = sock.recv(4096)
        sock.close()

        ''' Unpack the necessary information from the response '''
        # Unpack the header section of the response
        (header, current_pos) = myUnpack(HEADER_FORMAT, response)
        (ID, flags, num_questions, num_answers, num_auth, num_other) = header

        # Parse the flags
        (query_or_response, op_code, auth_ans, truncated, recur_desir,
                recur_avail, r_code) = parseFlags(flags)

        # Parse the question
        (question, current_pos) = myUnpack(QUERY_FORMAT, response, current_pos)
        (q_name, q_type, q_class) = question

        if (num_answers + num_auth + num_other == 0):
            print("No records found (Server fail)")
            return None
        

        # Parse the resource record
        (res_record, current_pos) = myUnpack(RECORD_FORMAT, response, current_pos)
        (rec_name, rec_type, rec_class, ttl, data_len) = res_record

        if (rec_type == A):
            # Parse IP address
            (res_data, current_pos) = myUnpack(IP_FORMAT, response, current_pos)

            # Format the IP address
            ip = ".".join(str(n) for n in res_data)

            print("Server response (Address record): the IP address is", ip + '!')
            return ip

        elif (rec_type == NS):
            return handleNSRecord(hostname, local_ip, response, current_pos, is_mx, num_auth, num_other, sock)                              

        elif (rec_type == CNAME):
            # Parse the canonical name
            (c_name, current_pos) = networkToString(response, current_pos)
            
            print("Server response (Canonical Name record):", c_name)

            IP = ROOT
            sock.close()
            return resolve(c_name, is_mx)

        elif (rec_type == MX):
            ((preference, exchange), current_pos) = myUnpack('HS', response, current_pos)

            print("Server response (Mail Exchange record):", exchange)
            return exchange

        elif (rec_type == SOA):
            print("SOA record found, bad query")
            return None

        else:
            print("Server response (Record type not supported):", rec_type)


    except socket.timeout as e:
        print("Server timed out :(")
        sock.close()
        return resolve(hostname, is_mx)

    return None


def selectIp():
    """
    Choose either a random IP from root-servers.txt or use the previous one.

    Returns:
        string: The chosen IP.
    """
    global IP, ROOT

    if (IP == None or IP == 0):
        # Choose a random IP from root-servers.txt
        new_file = open("root-servers.txt", "r")
        root_servers = [line.strip() for line in new_file]
        local_ip = root_servers[randint(0, len(root_servers) - 1)]

        ROOT = local_ip
        print("ROOT IP:", ROOT)
        new_file.close()
    else:
        # Use the IP from the previous recursive call
        local_ip = IP
        IP = None # reset

    return local_ip


def handleNSRecord(hostname, local_ip, response, current_pos, is_mx, num_auth, num_other, sock):
    """
    Parses the information from a nameserver record and attempts to resolve it
    to either an IP or address in the case of a MX query.

    Args:
        hostname (string): The original hostname being looked up.
        local_ip (string): the current IP we're looking into.
        response (string): The entire server message.
        current_pos (int): The current position withing the message.
        is_mx (bool): Whether the query was a mail exchange query
        num_auth (int): The number of authoritative records in the response.
        num_other (int): The number of additional records in the response.
    
    Returns:
        string: A string representation of an IP address (e.g. "192.168.0.1") or
          mail server (e.g. "mail.google.com"). If the request could not be
          resolved, None will be returned.
    """
    global IP
    global ROOT

    # Parse NS domain name
    (domain_name, current_pos) = myUnpack('S', response, current_pos)

    # If there are additional records
    if (num_other is not 0):
        # Parse the remaining authoritative records
        auths = []
        for x in range(num_auth - 1):
            (auth, current_pos) = myUnpack(RECORD_FORMAT + 'S', response, current_pos)
            auths.append(auth[-1])

        # Parse the additional section to find the record for the domain_name
        others = {} # A dictionary mapping a domain name to its ip
        for y in range(num_other):
            (other, current_pos) = myUnpack(RECORD_FORMAT, response, current_pos)

            if (other[1] == AAAA): # IPV6 records should be ignored
                (ignore, current_pos) = myUnpack('B'*16, response, current_pos)
                continue
                
            # Unpack the IP and add it to the dictionary
            (ip, current_pos) = myUnpack(IP_FORMAT, response, current_pos)
            others[other[0]] = ip

            # Stop once a matching record has been found
            if (domain_name in others):
                break
        
        # If the first auth record does not have a match, try another record
        if (domain_name not in others):
            for name in auths:
                if (name in others):
                    domain_name = name
                    break
        
        # Format the IP addresss
        IP = ".".join(str(n) for n in others[domain_name])
        print("Server response (Nameserver Record): The IP for", domain_name, "is", IP)

    # If there are no additional records
    else:
        print("\n\nServer response (Nameserver Record): no records found for IP of", domain_name)
        print("Searching for IP at", local_ip)
        print("\nSIDETRACK STARTS HERE-----------------------------------")

        # Restart the resolve at the ROOT to find the IP for the domain name
        IP = ROOT
        IP = resolve(domain_name)

        # No records found
        if (IP == None):
            print("\nSIDETRACK FOUND NO IP ASSOCIATED WITH", domain_name)
            return None
        
        print("\nSIDETRACK ENDS HERE-------------------------------------\n")
    
    # Continue the resolve
    sock.close()
    return resolve(hostname, is_mx)  


def myUnpack(formatStr, struct, start = 0):
    """
    An extension of struct.unpack that allows for string unpacking.

    Args:
        formatStr (string): a format string of the types of data to be parsed.
        struct (string): the entire network response message.
        start (int): the location within the message where the data starts.
            default value is the beginning of the message.

    Returns:
        A (list, int) tuple
            - list: a list of the unpacked values.
            - int: The index one past the end of the string, i.e. the starting
              index of the value immediately after the string.
    """
    assert len(formatStr) >= 1

    position = start
    ret = []
    # Parse the data for each character in the format string
    for c in formatStr:
        if c == 'S':
            val, position = networkToString(struct, position)
        else:
            val, position = networkToInteger(c, struct, position)

        ret.append(val)

    # If the data is of length 1, just return the data (not a list)
    if len(ret) == 1:
        ret = ret[0]

    return ret, position


def parseFlags(flags):
    """
    Parse the individual flags for the header section of a DNS message. These
    flags are found in the 2 bytes after the ID (message[2:5]).

    Args:
        flags (int): The flags stored as bits in a single integer.

    Returns:
        tuple(int): A list of the parsed flags in order they appear.
    """
    # The length of each flag in bits (reverse order)
    FLAG_LENGTHS = (7, 1, 1, 1, 1, 4, 1) # sum = 16 (or 2 bytes)
                 # /\__ the 4 response code bits plus the 3 reserved zero bits
                 #      parsing them simultaneously does not change the RCode
    
    flag_list = []
    for length in FLAG_LENGTHS:
        # Create a mask based on the length of the flag in bits
        mask = int('1' * length, 2) if length else 0

        flag_list.append(flags & mask) # mask off everything left of the flag
        flags >>= length # right shift to the next flag

    # Return the list of flags (reversing to put them in the correct order)
    return tuple(reversed(flag_list))


def main(argv):
    
    try:
        # Parsing the command line arguments
        assert len(argv) > 1
        assert len(argv) <= 3
    
        is_mx = '-m' in argv
        if (is_mx):
            argv.remove('-m')
            is_mx = True
        
        assert len(argv) == 2
        hostname = argv[1]

        # Resolve the hostname
        answer = resolve(hostname, is_mx)

        if answer is not None:
	        print(f"\nAnswer: {answer}")
        else:
	        print("Could not resolve request.")

    except AssertionError:
        usage(argv)


def usage(argv):
    """
    Print the correct usage.

    Args:
        argv (list): the arguments
    """
    print(f"Usage: python3 {argv[0]} [-m] <hostname>")

if __name__ == "__main__":
    main(sys.argv)
