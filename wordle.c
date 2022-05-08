// This module provides functions that can be used to play the game 
//   Wordle.



#include <string.h>
#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// assert_lower(guess, length) asserts all letters in guess are // 
//   lowercase.
// time: O(n)
static void assert_lower (const char *guess, int length) {
  for (int i = 0; i < length; i++) {
    char cur = guess[i];
    assert((cur >= 'a') && (cur <= 'z'));
  }
}

// assert_result(result, length) asserts all letters in result are 
//   either uppercase or lowercase or '.'.
// time: O(n)
static void assert_result (const char *result, int length) {
  for (int i = 0; i < length; i++) {
    char cur = result[i];
    assert(((cur >= 'a') && (cur <= 'z')) ||
           ((cur >= 'A') && (cur <= 'Z')) ||
           (cur == '.'));
  }
}

// see wordle.h
bool evaluate_guess(const char *secret, const char *guess, char *result) {
  int length_sec = strlen(secret);
  int length_guess = strlen(guess);

  assert(length_sec == length_guess);
  assert_lower(guess, length_guess);
  assert_result(result, strlen(result));
  // create two 0 int array to record the number of chars in each
  //   and one int array to record the common items. 
  // 30 is because there are 26 letters and 30 would give the 
  //   array sufficient length.
  int c_sec[30] = {0};
  int c_guess[30] = {0};
  int common[30] = {0};

  int length = length_sec;
  // loop secret
  for (int i = 0; i < length; i++) {
    char cur_s = secret[i]; // get the ascii value of current char
    c_sec[cur_s - 'a'] += 1; //increment this char in num array
  }

  // loop guess
  for (int j = 0; j < length; j++) {
    char cur_g = guess[j];
    c_guess[cur_g - 'a'] += 1;
  }


  // loop the two int arrays to find common letters
  for (int k = 0; k < 30; k++) {
    if ((c_sec[k] >= 1) && (c_guess[k] >= 1)) {
      common[k] = c_sec[k];
    }
  }

  // modify result
  for (int m = 0; m < length; m++) {
    int cur_index = guess[m] - 'a';

    // correct location
    if (secret[m] == guess[m]) {
      result[m] = guess[m] - 'a' + 'A'; // change to upper
    } else if (common[cur_index] != 0) { // exist but wrong place
      result[m] = guess[m];
    } else { // not exist (common[cur_index] == 0)
      result[m] = '.';
    }
  }

  // return true or false
  if (strcmp(secret, guess) == 0) {
    return true;
  } else {
    return false;
  }


}


// see wordle.h
int find_in_list(const char *guess, char *word_list[], int num_words) {
  int len_g = strlen(guess);
  assert_lower(guess, len_g);
  // binary search
  int len = num_words;
  int low = 0;
  int high = len - 1;
  while (low <= high) {
    int mid = (low + high) / 2;
    int is_equal = strcmp(guess, word_list[mid]);
    if (is_equal == 0) {
      return mid;
    } else if (is_equal > 0) {
      low = mid + 1;
    } else {
      high = mid - 1;
    }
  }
  return -1;
}


// see wordle.h
void available_letters(char *guesses[], char *results[], int num_guesses,
                       char *alphabet) {
  assert(num_guesses >= 0);

  // initialize the letters
  for (int i = 0; i < 26; i++) {
    alphabet[i] = 'a' + i;
  }

  for (int i = 0; i < num_guesses; i++) { // loop each guess
    assert(guesses[i]);
    assert(results[i]);
    int len_guess = strlen(guesses[i]);
    int len_res = strlen(results[i]);
    assert(len_guess == len_res);
    assert_lower(guesses[i], len_guess);
    assert_result(results[i], len_res);

    int len = len_guess;
    for (int j = 0; j < len; j++) { // loop letters in each guess
      if (results[i][j] == '.') { // not exist
        alphabet[guesses[i][j] - 'a'] = '.';
      } else { // exist
        alphabet[guesses[i][j] - 'a'] = guesses[i][j] - 'a' + 'A';
      }
    }
  }
  alphabet[26] = '\0'; // add the null terminator

}


// see wordle.h
bool valid_hard_guess(char *guesses[], char *results[], int num_guesses,
                      const char *next_guess) {
  assert(num_guesses >= 0);
  int len_next = strlen(next_guess);

  // loop guesses
  for (int i = 0; i < num_guesses; i++) {
    assert(guesses[i]);
    assert(results[i]);
    int len_g = strlen(guesses[i]);
    int len_r = strlen(results[i]);
    assert(len_g == len_r);
    assert_lower(guesses[i], len_g);
    assert_result(results[i], len_r);

    int len = len_g;
    assert(len == len_next);
    assert_lower(next_guess, len_next);

    bool forbid_c = false;
    bool same_p = true;
    bool is_in = false;
    // loop each item in guesses
    for (int j = 0; j < len; j++) {
      if (results[i][j] == '.') { // not exist
        char forbid = guesses[i][j];
        forbid_c = false;
        for (int k = 0; k < len_next; k++) {
          if (next_guess[k] == forbid) {
            forbid_c = true;
            return false; // once found forbid letter, return false
          }
        }
      } else if (results[i][j] <= 'Z') { // upper case
        if (next_guess[j] != guesses[i][j]) {
          same_p = false;
          return false;
          break;
        } 
      } else { // lower case
        if (next_guess[j] == results[i][j]) { // if same place
          return false;
          break;
        }

        // to see if the possible letter is presented but different
        //   place
        char letter = results[i][j];
        is_in = false;
        for (int k = 0; k < len_next; k++) { 
          if ((next_guess[k] == letter) && (k != j)) {
            is_in = true;
          }
        }
        if (is_in == false) {
          return false; // if letter not presented, return false
          break;
        }
      }
    }
  }

  return true;
}


// see wordle.h
int find_solution(char *guesses[], char *results[], int num_guesses,
                  char *word_list[], int num_words,
                  char *solutions[], int max_solutions) {
  //assert(max_solutions >= 1);
  assert(num_guesses >= 0);
  assert(max_solutions >= 1);

  // track which letter must exist, but placed in wrong place
  int must_have[30] = {0};
  int not_exist[30] = {0};
  // track possible solution-word index in word_list
  //   the actual length of sol will never exceed num_words.
  int sol[10000] = {0};
  // track if the solution word is possible
  bool is_possible = true;
  int len_word = strlen(word_list[0]);
  for (int i = 0; i < num_words; i++) { // loop words in word_list
    is_possible = true;
    len_word = strlen(word_list[i]);
    for (int j = 0; j < num_guesses; j++) { // loop each guess
      assert(guesses[j]);
      assert(results[j]);
      int len = strlen(guesses[j]);
      assert(len == len_word); // assert equal length
      int len_r = strlen(results[j]);
      assert(len == len_r);
      assert_lower(guesses[j], len);
      assert_result(results[j], len_r);
      
      for (int k = 0; k < len_word; k++) { // loop each letter
        if (results[j][k] == '.') { // letter not exist
          not_exist[guesses[j][k] - 'a'] = 1;
          if ((word_list[i][k] != guesses[j][k]) && is_possible) {
            sol[i] = 1;
          } else {
            sol[i] = 0;
            is_possible = false; // this word in list is impossible
            break; // this guess is not possible
          }
        } else if (results[j][k] <= 'Z') { // letter in right place
          if ((word_list[i][k] == guesses[j][k]) && is_possible) {
            sol[i] = 1;
          } else {
            sol[i] = 0;
            is_possible = false;
            break;
          }
        } else { // letter in wrong place
          if ((word_list[i][k] != guesses[j][k]) && is_possible) {
            sol[i] = 1;
            // modify must_have
            must_have[guesses[j][k] - 'a'] = 1;
          } else {
            sol[i] = 0;
            is_possible = false;
            break;
          }
        }
      }
    }
  }

  // check if the possible solution list contains the must-have
  //   letter and cannot contain letter in not_exist
  bool have_must = false;
  bool exist_not = false;
  for (int i = 0; i < num_words; i++) {
    if (sol[i] != 0) {
      // check must_have
      for (int j = 0; j < 30; j++) {
        if (must_have[j] != 0) {
          have_must = false;
          for (int k = 0; k < len_word; k++) {
            if (word_list[i][k] == j + 'a') { 
              have_must = true;
            }
          }
          if (have_must == false) {
            sol[i] = 0;
          }
        }
      }
      // check not_exist
      for (int j = 0; j < 30; j++) {
        if (not_exist[j] != 0) {
          exist_not = false;
          for (int k = 0; k < len_word; k++) {
            if (word_list[i][k] == j + 'a') {
              exist_not = true;
            }
          }
          if (exist_not) { // if it has letter that shouldn't exist
            sol[i] = 0;
          }
        }
      }
    }
  }

  // use the index based on sol array to modify solutions
  int solution_index = 0;
  for (int i = 0; i < num_words; i++) {
    if (solution_index == max_solutions) {
      break;
    }
    if (sol[i] != 0) {
      solutions[solution_index] = word_list[i];
      solution_index++;
    }
  }

  // return the number of possible solutions
  return solution_index;
}
