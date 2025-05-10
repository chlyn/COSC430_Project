# COSC430 Final Project

A Unix-style shell with basic job control, implemented for COSC 430.

---

## Features Implemented

- **SIGINT (`^C`) handling**  
  Pressing <kbd>Ctrl</kbd>+<kbd>C</kbd> will terminate *only* the foreground job, without exiting the shell itself.

- **SIGTSTP (`^Z`) handling**  
  Pressing <kbd>Ctrl</kbd>+<kbd>Z</kbd> will suspend (stop) the foreground job and return control to the shell.

- **`jobs` command**  
  Lists all current background and stopped jobs, showing job IDs, PIDs, states, and command names.
  
- **`help` command**  
  Lists all commands in a user friendly display
  ![image](https://github.com/user-attachments/assets/4ad2e4d2-04d7-4bb9-84cf-1601c7c2f15c)

- **`bg <job>` command**
  Resumes a stopped job in the background

- **`fg <job>` command**
  Resumes a stopped job or currently running background job in the foreground

> **Work in progress:**  
> The following built-in commands still need to be implemented:  
> - `kill <job>`: Terminate a job 

---

## Scenario Screenshots

-  **Scenario 1**
  <br> Checking if ^C kills the current process.
  ![image](https://github.com/user-attachments/assets/6b73cb6f-74b6-4aeb-8202-aace25844e35)

- **Scenario 2**
  <br> Checking if ^Z stops the current process.
  ![image](https://github.com/user-attachments/assets/735ea40c-2f4f-44cc-a739-d1421a8ddb69)
  
- **Scenario 3**
  <br> Checking if after ^C is pressed. Jobs will show it has been removed from the JobsList.
  ![image](https://github.com/user-attachments/assets/7226aba5-bedd-472c-9fa1-435449da3bf4)

- **Scenario 4**
  <br> Checking if after ^Z is pressed. Jobs will show it stopped and converted to BACKGROUND in the JobsList.
  ![image](https://github.com/user-attachments/assets/9af03d9c-ee70-47e9-a3be-c0fdd29258f8)

- **Scenario 5**
  <br>Checking if after running `fg <job>` on a stopped job or currently running background process, it resumes in the foreground and waits for completion.
  ![image](https://github.com/user-attachments/assets/d15ec4b7-e8be-4089-b22d-f1090924d91c)

- **Scenario 6**
  <br>Checking if after running `bg <job>` on a stopped job. Jobs will show it has resumed and converted to BACKGROUND in the JobsList.
  ![image](https://github.com/user-attachments/assets/8e8fe96b-8a89-4767-899f-f704b869579f)
