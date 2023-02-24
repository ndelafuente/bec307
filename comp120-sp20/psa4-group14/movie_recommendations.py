"""
Name: movie_recommendations.py
Date: March 23rd, 2020
Author: Nico de la Fuente and Katrina Baha
Description: A program that predicts the rating a user will give to
             a specified movie based on past ratings
"""

import math
import csv
from scipy.stats import pearsonr

class BadInputError(Exception):
    pass

class Movie_Recommendations:
    # Constructor
    def __init__(self, movie_filename, training_ratings_filename):
        """
        Initializes the Movie_Recommendations object from 
        the files containing movie names and training ratings.  
        The following instance variables should be initialized:
        self.movie_dict - A dictionary that maps a movie id to
               a movie objects (objects the class Movie)
        self.user_dict - A dictionary that maps user id's to a 
               a dictionary that maps a movie id to the rating
               that the user gave to the movie.    
        """

        # Initialize the dictionaries
        self.movie_dict = {}
        self.user_dict = {}

        # Process movie file
        file = open(movie_filename, 'r')
        file.readline() # ignore the header
        csv_reader = csv.reader(file, delimiter = ',', quotechar = '"')
        for line in csv_reader:
            # Parse the line
            movie_id = int(line[0])
            title = line[1]

            # Update the dictionary
            self.movie_dict[movie_id] = Movie(movie_id, title)

        # Close the file
        file.close()


        # Process ratings file
        file = open(training_ratings_filename, 'r')
        file.readline() # ignore the header
        csv_reader = csv.reader(file, delimiter = ',', quotechar = '"')
        for line in csv_reader:
            # Parse the line
            user_id = int(line[0])
            movie_id = int(line[1])
            rating = float(line[2])

            # Update the dictionaries
            self.movie_dict[movie_id].users.append(user_id)
            self.user_dict.setdefault(user_id, {}) # initializes the dictionary if user_id has not been set
            self.user_dict[user_id][movie_id] = rating

        # Close the file
        file.close()



    def predict_rating(self, user_id, movie_id):
        """
        Returns the predicted rating that user_id will give to the
        movie whose id is movie_id. 
        If user_id has already rated movie_id, return
        that rating.
        If either user_id or movie_id is not in the database,
        then BadInputError is raised.
        """

        # Checks for bad input
        if user_id not in self.user_dict or movie_id not in self.movie_dict:
            raise BadInputError

        # Returns the user's rating if the user has already rated the movie
        elif movie_id in self.user_dict[user_id]:
            return self.user_dict[user_id][movie_id]

        # Computes the predicted rating
        else:
            product_sum = 0
            similarity_sum = 0

            # Passes over the other movies the user has previously watched
            for prev_watched_movie in self.user_dict[user_id]:
                similarity = self.movie_dict[movie_id].get_similarity(prev_watched_movie, 
                    self.movie_dict, self.user_dict)
                similarity_sum += similarity
                product_sum += similarity  * self.user_dict[user_id][prev_watched_movie]
            
            # If nobody has watched the movies
            if similarity_sum == 0:
                rating_prediction = 2.5 # Return an average rating

            # Otherwise calculate the predicted rating
            else:
                rating_prediction = product_sum / similarity_sum
            
            return rating_prediction


    def predict_ratings(self, test_ratings_filename):
        """
        Returns a list of tuples, one tuple for each rating in the
        test ratings file.
        The tuple should contain
        (user id, movie title, predicted rating, actual rating)
        """

        # Open and parse the file
        filename = open(test_ratings_filename, 'r')
        filename.readline() # ignore the header
        csv_reader = csv.reader(filename, delimiter = ',', quotechar = '"')
        
        # Predict ratings for each movie
        ratings_list = []
        for line in csv_reader:
            user_id = int(line[0])
            movie_id = int(line[1])
            movie_title = self.movie_dict[movie_id].title
            actual_rating = float(line[2])
            predicted_rating = self.predict_rating(user_id, movie_id)

            ratings_list.append((user_id, movie_title, predicted_rating, actual_rating))

        return ratings_list

    def correlation(self, predicted_ratings, actual_ratings):
        """
        Returns the correlation between the values in the list predicted_ratings
        and the list actual_ratings.  The lengths of predicted_ratings and
        actual_ratings must be the same.
        """

        return pearsonr(predicted_ratings, actual_ratings)[0]
        
class Movie: 
    """
    Represents a movie from the movie database.
    """
    def __init__(self, id, title):
        """ 
        Constructor.
        Initializes the following instances variables.  You
        must use exactly the same names for your instance 
        variables.  (For testing purposes.)
        id: the id of the movie
        title: the title of the movie
        users: list of the id's of the users who have
            rated this movie.  Initially, this is
            an empty list, but will be filled in
            as the training ratings file is read.
        similarities: a dictionary where the key is the
            id of another movie, and the value is the similarity
            between the "self" movie and the movie with that id.
            This dictionary is initially empty.  It is filled
            in "on demand", as the file containing test ratings
            is read, and ratings predictions are made.
        """
        
        self.id = id
        self.title = title

        self.users = []
        self.similarities = {}

    def get_similarity(self, other_movie_id, movie_dict, user_dict):
        """ 
        Returns the similarity between the movie that 
        called the method (self), and another movie whose
        id is other_movie_id.  (Uses movie_dict and user_dict)
        If the similarity has already been computed, return it.
        If not, compute the similarity (using the compute_similarity
        method), and store it in both
        the "self" movie object, and the other_movie_id movie object.
        Then return that computed similarity.
        If other_movie_id is not valid, raise BadInputError exception.
        """

        # Checks for bad input
        if other_movie_id not in movie_dict:
            raise BadInputError

        # If the similarity has already been computed return it
        if self.similarities.get(other_movie_id) != None:
            return self.similarities.get(other_movie_id)

        # Otherwise compute and return the similarity
        else:
            similarity = self.compute_similarity(other_movie_id, movie_dict, user_dict)

            # Adding similarity to each movie's dictionary
            self.similarities[other_movie_id] = similarity
            movie_dict[other_movie_id].similarities[self.id] = similarity

            return similarity
        

    def compute_similarity(self, other_movie_id, movie_dict, user_dict):
        """ 
        Computes and returns the similarity between the movie that 
        called the method (self), and another movie whose
        id is other_movie_id.  (Uses movie_dict and user_dict)
        """
        
        # Calculate the differences between the ratings of the two movies for every user
        list_of_differences = []
        for user in user_dict:
            movie1_rating = user_dict[user].get(self.id)
            movie2_rating = user_dict[user].get(other_movie_id)

            if movie1_rating != None and movie2_rating != None:
                list_of_differences.append(abs(movie1_rating - movie2_rating))
        
        # If there are no differences
        if len(list_of_differences) == 0:
            similarity = 0 # Then nobody has watched both movies

        # Otherwise compute the similarity
        else:
            avg_diferences = sum(list_of_differences) / len(list_of_differences)
            similarity = 1.0 - avg_diferences / 4.5

        return similarity

    def __str__(self):
        """
        Returns string representation of the movie object.
        Handy for debugging.
        """
        
        return("Movie Title: " + self.title + "Movie ID: " + self.id)

    def __repr__(self):
        """
        Returns string representation of the movie object.
        """
        return("Movie Title: " + self.title + "Movie ID: " + self.id)
        


if __name__ == "__main__":
    # Create movie recommendations object.
    movie_recs = Movie_Recommendations("movies.csv", "training_ratings.csv")

    # Predict ratings for user/movie combinations
    rating_predictions = movie_recs.predict_ratings("test_ratings.csv")
    print("Rating predictions: ")
    for prediction in rating_predictions:
        print(prediction)
    predicted = [rating[2] for rating in rating_predictions]
    actual = [rating[3] for rating in rating_predictions]
    correlation = movie_recs.correlation(predicted, actual)
    print(f"Correlation: {correlation}")    