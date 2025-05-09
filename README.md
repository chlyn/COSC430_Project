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

> **Work in progress:**  
> The following built-in commands still need to be implemented:  
> - `bg <job>`: Resume a stopped job in the background  
> - `fg <job>`: Bring a job to the foreground  
> - `kill <job>`: Terminate a job 

---

## Scenario Screenshots

-  **Scenario 1**
  <br> Checking if ^C kills the current process
  ![image](https://github.com/user-attachments/assets/6b73cb6f-74b6-4aeb-8202-aace25844e35)

- **Scenario 2**
  <br> Checking if ^Z stops the current process
  ![image](https://github.com/user-attachments/assets/735ea40c-2f4f-44cc-a739-d1421a8ddb69)
  
- **Scenario 3**
  <br> Checking if after ^C is pressed. Jobs will show it removed from the JobsList
  ![image](https://github.com/user-attachments/assets/7226aba5-bedd-472c-9fa1-435449da3bf4)

- **Scenario 4**
  <br> Checking if after ^Z is pressed. Jobs will show it stopped and converted to BACKGROUND in the JobsList
  ![image](https://github.com/user-attachments/assets/9af03d9c-ee70-47e9-a3be-c0fdd29258f8)



