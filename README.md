# Spotify Songs Recommendation System

This project implements a recommendation system for Spotify songs using C programming language, leveraging hash table and max heap data structures to efficiently manage and recommend songs based on various musical attributes.

## Overview

The system reads a dataset of Spotify songs from a CSV file, stores the song information in a hash table for quick retrieval, and then allows users to input a track ID to receive a list of recommended songs that share similar musical characteristics (such as danceability, energy, tempo, etc.) and belong to different artists.

The recommendation system is designed to assist users in discovering new songs that align with their musical preferences based on the attributes of a selected track.

## Features

- **Hash Table Implementation**: The hash table is used to store Spotify song data, allowing for O(1) average time complexity for insertions and lookups.
  
- **Max Heap for Recommendations**: A max heap data structure is employed to manage and prioritize recommended songs based on their popularity and similarity to the input track.

- **Song Recommendation Logic**: Songs are recommended based on similarity in musical attributes (danceability, energy, etc.) and artist diversity, ensuring a diverse set of recommended tracks.

## Setup

Follow these steps to set up and run the Spotify Songs Recommendation System on your local machine:

1. **Clone the Repository**:

   ```bash
   git clone https://github.com/yourusername/spotify-songs-recommendation.git

cd spotify-songs-recommendation
gcc -o recommendation main.c

./recommendation
