## Instructions for Compilation and Execution

1. **Compile and Run the Code:**
   To compile all the files and execute the program, use the following command:
   ```
   make run
   ```

2. **Clean Project Directory:**
   To remove the object files and clean up the project directory, use the command:
   ```
   make clean
   ```

3. **Customize Time Slice for Round Robin:**
   The default time slice for Round Robin is set to 5. To customize the time slice for Round Robin, use the following command:
   ```
   make run RR_TIME_SLICE="10"
   ```
Replace "10" with your desired time slice value.