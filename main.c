#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>

#define MAX_WORDS 5757
#define WORD_LENGTH 5
#define NUM_THREADS 6

char *words[MAX_WORDS];
int global_max_score = 0;
char best_words[3][WORD_LENGTH + 1];
pthread_mutex_t score_mutex = PTHREAD_MUTEX_INITIALIZER;

void readWordsFromFile(const char *filename)
{
  FILE *file = fopen(filename, "r");
  if (!file)
  {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  char line[WORD_LENGTH + 2]; // +2 for newline and null terminator
  int i = 0;
  while (fgets(line, sizeof(line), file) && i < MAX_WORDS)
  {
    line[strcspn(line, "\n")] = 0; // Remove newline character
    words[i] = strdup(line);
    i++;
  }

  fclose(file);
}

void compareWordsAndCalculateScore(char inputWords[3][WORD_LENGTH + 1], int *local_score)
{
  for (int i = 0; i < MAX_WORDS; i++)
  {
    int score = 0;
    int correctLetters[26] = {0};
    int charIndices[26][WORD_LENGTH] = {{0}};

    for (int j = 0; j < 3; j++)
    {
      for (int k = 0; k < WORD_LENGTH; k++)
      {
        if (inputWords[j][k] == words[i][k] && !correctLetters[inputWords[j][k] - 'a'])
        {
          score += 3;
          correctLetters[inputWords[j][k] - 'a'] = 1;
        }
        else if (strchr(words[i], inputWords[j][k]) &&
                 !charIndices[inputWords[j][k] - 'a'][k] &&
                 !correctLetters[inputWords[j][k] - 'a'])
        {
          score += 1;
          charIndices[inputWords[j][k] - 'a'][k] = 1;
        }
      }
    }
    *local_score += score;
  }
}

void *findBestScore(void *arg)
{
  int thread_id = *(int *)arg;
  int local_max_score = 0;
  char local_best_words[3][WORD_LENGTH + 1];
  int numOfIter = 0;

  for (int i = thread_id; i < 498; i += NUM_THREADS)
  {
    for (int j = i + 1; j < 499; j++)
    {
      for (int k = j + 1; k < 500; k++)
      {
        char inputWords[3][WORD_LENGTH + 1];
        strcpy(inputWords[0], words[i]);
        strcpy(inputWords[1], words[j]);
        strcpy(inputWords[2], words[k]);

        int local_score = 0;
        compareWordsAndCalculateScore(inputWords, &local_score);
        if (local_score > local_max_score)
        {
          local_max_score = local_score;
          strcpy(local_best_words[0], words[i]);
          strcpy(local_best_words[1], words[j]);
          strcpy(local_best_words[2], words[k]);
        }
        numOfIter++;
      }
    }
  }

  pthread_mutex_lock(&score_mutex);
  if (local_max_score > global_max_score)
  {
    global_max_score = local_max_score;
    strcpy(best_words[0], local_best_words[0]);
    strcpy(best_words[1], local_best_words[1]);
    strcpy(best_words[2], local_best_words[2]);
  }
  pthread_mutex_unlock(&score_mutex);

  printf("Thread %d completed %d iterations\n", thread_id, numOfIter);
  return NULL;
}

int main()
{
  readWordsFromFile("sgb-words.txt");

  pthread_t threads[NUM_THREADS];
  int thread_ids[NUM_THREADS];

  for (int i = 0; i < NUM_THREADS; i++)
  {
    thread_ids[i] = i;
    pthread_create(&threads[i], NULL, findBestScore, &thread_ids[i]);
  }

  for (int i = 0; i < NUM_THREADS; i++)
  {
    pthread_join(threads[i], NULL);
  }

  printf("Best word combo: %s, %s, and %s\n", best_words[0], best_words[1], best_words[2]);
  printf("Max score: %d\n", global_max_score);

  return 0;
}
