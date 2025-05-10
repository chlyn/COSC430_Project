# COSC430 Final Project

A custom Unix-style shell, implemented for COSC 430. This shell replicates basic job control behavior found in modern shells like bash, allowing users to execute, stop, resume, and manage foreground and background processes using signal handlers and built-in commands.

---

## Group Members
- Oluwabukunmi David Jaiyeoloa
- Chenilyn Joy Espineda
- Dayana Ferrufino

---

## Features Implemented

- **SIGINT (`^C`) handling**  
  Pressing <kbd>Ctrl</kbd>+<kbd>C</kbd> will terminate *only* the foreground job, without exiting the shell itself and immediately prompts for the next command.

- **SIGTSTP (`^Z`) handling**  
  Pressing <kbd>Ctrl</kbd>+<kbd>Z</kbd> will suspend (stop) the foreground job, then moved to the background in a stopped state and immediately prompts for the next command.

- **`jobs` command**  
  Lists all current background and stopped jobs, showing each job's job IDs, PIDs, states  (Running or Stopped), and command names:
  ![image](https://github.com/user-attachments/assets/cd40770a-b636-4b7e-958f-2e722888e039)

  
- **`help` command**  
  Lists all commands and key signal behaviors in a user-friendly display:
  ![image](https://github.com/user-attachments/assets/e93a14a8-e285-46b4-8cff-00e6509f478e)

- **`fg <job>` command**
  <br>Moves a stopped or running background job to the foreground and updates its state to RUNNING and its location to FOREGROUND.

  - **`bg <job>` command**
  <br>Resumes a stopped job in the background and updates its state to RUNNING.

> **Work in progress:**  
> The following built-in commands still need to be implemented:  
> - `kill <job>`: Terminate a job 

---

## Scenario Screenshots

-  **Scenario 1**
  <br> Checking if ^C kills only the current foreground process.
  ![image](https://github.com/user-attachments/assets/6b73cb6f-74b6-4aeb-8202-aace25844e35)

- **Scenario 2**
  <br> Checking if ^Z stops the current foreground process.
  ![image](https://github.com/user-attachments/assets/735ea40c-2f4f-44cc-a739-d1421a8ddb69)
  
- **Scenario 3**
  <br> Verifying if after ^C is pressed, Jobs will no longer list the terminated process in the JobsList.
  ![image](https://github.com/user-attachments/assets/7226aba5-bedd-472c-9fa1-435449da3bf4)

- **Scenario 4**
  <br> Verifying if after ^Z is pressed, Jobs will show it as STOPPED and converted to BACKGROUND in the JobsList.
  ![image](https://github.com/user-attachments/assets/9af03d9c-ee70-47e9-a3be-c0fdd29258f8)

- **Scenario 5**
  <br>Verifying if after running `fg <job>` on a stopped job or currently running background process, it resumes in the foreground and Jobs will show it as RUNNING and converted to FOREGROUND in the JobsList.
  ![image](https://github.com/user-attachments/assets/f4daba86-32bf-495f-8c1d-912f80cd24b3)

- **Scenario 6**
  <br>Checking if after running `bg <job>` on a stopped job, it resumes in the background and Jobs will show it as RUNNING in the JobsList.
  ![image](https://github.com/user-attachments/assets/8e8fe96b-8a89-4767-899f-f704b869579f)
