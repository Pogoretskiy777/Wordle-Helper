const fs = require("fs");
const readline = require("readline");

// Function to read file and return the list of words
function readWordsFromFile(filename) {
  const data = fs.readFileSync(filename, "utf8");
  const words = data.split("\n").map((word) => word.trim()); // Trim to handle any extra whitespace
  Object.freeze(words);
  return words;
}

// List of words from the file
const words = readWordsFromFile("sgb-words.txt");

// Global score variable
let score = 0;

// Create readline interface for user input
const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout,
});

// Function to prompt user for input and validate it
function promptForWord(promptText, callback) {
  rl.question(promptText, (input) => {
    const lowercasedInput = input.toLowerCase();
    if (words.includes(lowercasedInput)) {
      callback(lowercasedInput);
    } else {
      console.log("Invalid input. Please enter a valid word from the list.");
      promptForWord(promptText, callback);
    }
  });
}

// Function to compare words and update the score
function compareWordsAndCalculateScore(inputWords, wordList) {
  wordList.forEach((listWord) => {
    let correctLetters = new Set();
    let charIndices = new Map();

    inputWords.forEach((inputWord) => {
      for (let i = 0; i < 5; i++) {
        if (inputWord[i] == listWord[i] && !correctLetters.has(inputWord[i])) {
          score += 3;
          correctLetters.add(inputWord[i]);
        } else if (
          listWord.includes(inputWord[i]) &&
          (!charIndices.has(inputWord[i]) || !charIndices.get(inputWord[i]).includes(i)) &&
          !correctLetters.has(inputWord[i])
        ) {
          score += 1;
          if (!charIndices.has(inputWord[i])) {
            charIndices.set(inputWord[i], [i]);
          } else {
            charIndices.get(inputWord[i]).push(i);
          }
        }
      }
    });
  });
}

// Main function to get three words from the user and calculate the score
function getThreeWords() {
  let firstWord = "trees",
    secondWord = "trees",
    thirdWord = "trees";

  rl.close();
  console.log(`First word: ${firstWord}`);
  console.log(`Second word: ${secondWord}`);
  console.log(`Third word: ${thirdWord}`);

  // Compare words and calculate the score
  compareWordsAndCalculateScore([firstWord, secondWord, thirdWord], words);

  console.log(`Total Score: ${score}/${words.length * 15}`);
}

// Start the process
getThreeWords();