#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#define MAX_TRACKS 30000
#define MAX_NAME_LENGTH 100
#define HASH_TABLE_SIZE 47737
#define RECOMMENDED_SONGS 10

typedef struct {
    char track_id[MAX_NAME_LENGTH];
    char track_name[MAX_NAME_LENGTH];
    char track_artist[MAX_NAME_LENGTH];
    char playlist_genre[MAX_NAME_LENGTH];
    char playlist_subgenre[MAX_NAME_LENGTH];
    double danceability;
    double energy;
    double loudness;
    double speechiness;
    double acousticness;
    double liveness;
    double valence;
    double tempo;
    int track_popularity;
    int duration_ms;
} Track;

typedef struct HashNode {
    Track* track;
    struct HashNode* next;
} HashNode;

typedef struct {
    HashNode* buckets[HASH_TABLE_SIZE];
} HashTable;

int hashFunction(const char* track_id) {
    unsigned long hash = 5381;
    int c;

    while ((c = *track_id++)) {
        hash = ((hash << 5) + hash) ^ c; /* DJB2 hash */
    }

    return hash % HASH_TABLE_SIZE;
}

void insertIntoHashTable(HashTable* table, Track* newTrack) {
    int index = hashFunction(newTrack->track_id);

    HashNode* newNode = (HashNode*)malloc(sizeof(HashNode));
    newNode->track = newTrack;
    newNode->next = NULL;

    if (table->buckets[index] == NULL) {
        table->buckets[index] = newNode;
    } else {
        HashNode* current = table->buckets[index];
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

typedef struct {
    Track** heapArray;
    int size;
    int capacity;
    bool* recommended;
} MaxHeap;

MaxHeap* createMaxHeap(int capacity) {
    MaxHeap* maxHeap = (MaxHeap*)malloc(sizeof(MaxHeap));
    maxHeap->heapArray = (Track**)malloc(sizeof(Track*) * (capacity + 1));
    maxHeap->size = 0;
    maxHeap->capacity = capacity;
    maxHeap->recommended = (bool*)calloc(MAX_TRACKS, sizeof(bool));
    return maxHeap;
}

void swap(Track** a, Track** b) {
    Track* temp = *a;
    *a = *b;
    *b = temp;
}

void insertMaxHeap(MaxHeap* maxHeap, Track* track) {
    if (maxHeap->size == maxHeap->capacity) {
        printf("Max Heap is full. Cannot insert more tracks.\n");
        return;
    }

    maxHeap->size++;
    int i = maxHeap->size;

    maxHeap->heapArray[i] = track;

    while (i > 1 && maxHeap->heapArray[i]->track_popularity > maxHeap->heapArray[i / 2]->track_popularity) {
        swap(&maxHeap->heapArray[i], &maxHeap->heapArray[i / 2]);
        i = i / 2;
    }
}

Track* extractMax(MaxHeap* maxHeap) {
    if (maxHeap->size == 0) {
        printf("Max Heap is empty.\n");
        return NULL;
    }

    Track* maxTrack = maxHeap->heapArray[1];
    maxHeap->heapArray[1] = maxHeap->heapArray[maxHeap->size];
    maxHeap->size--;

    int idx = 1;
    while (idx * 2 <= maxHeap->size) {
        int maxIdx = idx;
        int leftChild = idx * 2;
        int rightChild = idx * 2 + 1;

        if (leftChild <= maxHeap->size && maxHeap->heapArray[leftChild]->track_popularity > maxHeap->heapArray[idx]->track_popularity) {
            maxIdx = leftChild;
        }
        if (rightChild <= maxHeap->size && maxHeap->heapArray[rightChild]->track_popularity > maxHeap->heapArray[maxIdx]->track_popularity) {
            maxIdx = rightChild;
        }

        if (maxIdx != idx) {
            swap(&maxHeap->heapArray[idx], &maxHeap->heapArray[maxIdx]);
            idx = maxIdx;
        } else {
            break;
        }
    }

    return maxTrack;
}

void heapify(MaxHeap* maxHeap, int idx) {
    int leftChild = 2 * idx;
    int rightChild = 2 * idx + 1;
    int largest = idx;

    if (leftChild <= maxHeap->size && maxHeap->heapArray[leftChild]->track_popularity > maxHeap->heapArray[idx]->track_popularity) {
        largest = leftChild;
    }
    if (rightChild <= maxHeap->size && maxHeap->heapArray[rightChild]->track_popularity > maxHeap->heapArray[largest]->track_popularity) {
        largest = rightChild;
    }

    if (largest != idx) {
        swap(&maxHeap->heapArray[idx], &maxHeap->heapArray[largest]);
        heapify(maxHeap, largest);
    }
}

Track* getTrackFromHashTable(HashTable* table, const char* track_id); // Declaration moved here

void recommendSongs(HashTable* table, MaxHeap* maxHeap, const char* track_id) {
    Track* selectedTrack = getTrackFromHashTable(table, track_id);
    if (selectedTrack == NULL) {
        printf("Track with ID %s not found.\n", track_id);
        return;
    }

    // Print information of the currently playing song
    printf("Currently playing\n");
    printf("Track Name: %s\n", selectedTrack->track_name);
    printf("Artist: %s\n", selectedTrack->track_artist);
    printf("Genre: %s\n", selectedTrack->playlist_genre);
    printf("Subgenre: %s\n\n", selectedTrack->playlist_subgenre);

    printf("Recommended Songs Playlist \n\n");

    int totalDuration = 0;

    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode* current = table->buckets[i];
        while (current != NULL) {
            Track* track = current->track;
            if (strcmp(track->track_artist, selectedTrack->track_artist) != 0 &&
                fabs(track->danceability - selectedTrack->danceability) <= 0.2 &&
                fabs(track->energy - selectedTrack->energy) <= 0.2 &&
                fabs(track->loudness - selectedTrack->loudness) <= 5.0 &&
                fabs(track->speechiness - selectedTrack->speechiness) <= 0.2 &&
                fabs(track->acousticness - selectedTrack->acousticness) <= 0.2 &&
                fabs(track->liveness - selectedTrack->liveness) <= 0.2 &&
                fabs(track->valence - selectedTrack->valence) <= 0.2 &&
                fabs(track->tempo - selectedTrack->tempo) <= 10.0 &&
                strcmp(track->track_id, track_id) != 0 &&
                !maxHeap->recommended[atoi(track->track_id)]) {

                insertMaxHeap(maxHeap, track);
                maxHeap->recommended[atoi(track->track_id)] = true;
            }
            current = current->next;
        }
    }

    for (int i = 0; i < RECOMMENDED_SONGS && maxHeap->size > 0; i++) {
        Track* topTrack = extractMax(maxHeap);
        printf("track_name: %s\n", topTrack->track_name);
        printf("track_artist: %s\n", topTrack->track_artist);
        printf("playlist_genre: %s\n", topTrack->playlist_genre);
        printf("playlist_subgenre: %s\n", topTrack->playlist_subgenre);
        printf("\n");

        totalDuration += topTrack->duration_ms;
    }

    // Convert totalDuration from milliseconds to hours:minutes:seconds format
    int seconds = totalDuration / 1000;
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    seconds = seconds % 60;

    printf("Total duration of playlist is (%02d:%02d:%02d)\n", hours, minutes, seconds);
}

Track* getTrackFromHashTable(HashTable* table, const char* track_id) {
    int index = hashFunction(track_id);
    HashNode* current = table->buckets[index];

    while (current != NULL) {
        if (strcmp(current->track->track_id, track_id) == 0) {
            return current->track;
        }
        current = current->next;
    }

    return NULL;
}

int readTracksAndBuildHashTable(const char* filename, HashTable* table) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return 0;
    }

    char line[1024];
    int count = 0;

    while (fgets(line, sizeof(line), file)) {
        if (count >= MAX_TRACKS) {
            printf("Warning: Maximum tracks limit reached (%d tracks)\n", MAX_TRACKS);
            break;
        }

        Track* newTrack = (Track*)malloc(sizeof(Track));
        char* token = strtok(line, ",");
        int tokenIndex = 0;

        while (token != NULL && tokenIndex < 15) {
            switch (tokenIndex) {
                case 0: strncpy(newTrack->track_id, token, sizeof(newTrack->track_id) - 1); break;
                case 1: strncpy(newTrack->track_name, token, sizeof(newTrack->track_name) - 1); break;
                case 2: strncpy(newTrack->track_artist, token, sizeof(newTrack->track_artist) - 1); break;
                case 3: newTrack->track_popularity = atoi(token); break;
                case 4: strncpy(newTrack->playlist_genre, token, sizeof(newTrack->playlist_genre) - 1); break;
                case 5: strncpy(newTrack->playlist_subgenre, token, sizeof(newTrack->playlist_subgenre) - 1); break;
                case 6: newTrack->danceability = atof(token); break;
                case 7: newTrack->energy = atof(token); break;
                case 8: newTrack->loudness = atof(token); break;
                case 9: newTrack->speechiness = atof(token); break;
                case 10: newTrack->acousticness = atof(token); break;
                case 11: newTrack->liveness = atof(token); break;
                case 12: newTrack->valence = atof(token); break;
                case 13: newTrack->tempo = atof(token); break;
                case 14: newTrack->duration_ms = atoi(token); break;
                default: break;
            }
            token = strtok(NULL, ",");
            tokenIndex++;
        }

        insertIntoHashTable(table, newTrack);
        count++;
    }

    fclose(file);
    return count;
}

void handleUserInput(HashTable* table, MaxHeap* maxHeap) {
    char track_id[MAX_NAME_LENGTH];
    printf("Enter track ID: ");
    scanf("%s", track_id);

    recommendSongs(table, maxHeap, track_id);
}

int main() {
    
    HashTable trackTable = {0};
    MaxHeap* maxHeap = createMaxHeap(MAX_TRACKS);
   
    int num_tracks = readTracksAndBuildHashTable("C:/Users/Suraj/Downloads/Final_DSA/spotify_songs.csv", &trackTable);
    if (num_tracks == 0) {
        printf("Error: No tracks found or unable to read CSV file.\n");
        return 1;
    }
  
    handleUserInput(&trackTable, maxHeap);

    // Free allocated memory
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode* current = trackTable.buckets[i];
        while (current != NULL) {
            HashNode* next = current->next;
            free(current->track);
            free(current);
            current = next;
        }
    }
    free(maxHeap->heapArray);
    free(maxHeap->recommended);
    free(maxHeap);

    return 0;
}
