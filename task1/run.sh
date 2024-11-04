#!/bin/bash

# This script automates the process of setting up the environment,
# building the solution, and running the evaluation.

# Step 1: Set up environment variables
source source.sh

# Check that TECHARENA24_TASK1_DIR is set
if [[ -z "$TECHARENA24_TASK1_DIR" ]]; then
    echo "The TECHARENA24_TASK1_DIR bash environment variable is not set."
    echo "To set the TECHARENA24_TASK1_DIR variable, please cd to task1 directory and"
    echo "run the command 'source source.sh'."
    exit 1
fi

# Step 3: Build the solution
./scripts/build.sh

# Step 4: Evaluate the solution
./scripts/evaluate.sh -r routes/route.txt