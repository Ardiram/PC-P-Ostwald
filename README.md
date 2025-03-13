# Docker: A Comprehensive Overview

## What is Docker?

Docker is an open-source platform designed to automate the development, deployment, and management of applications within lightweight, portable, and self-sufficient containers. It allows developers to package applications and their dependencies into a standardized unit for software development.

Docker is widely used for its ability to:

- Simplify application deployment processes.
- Ensure consistency across multiple environments (development, testing, production).
- Increase resource efficiency compared to traditional virtual machines.

---

## What is Docker Used For?

Docker is used for various purposes, including:

1. **Application Deployment**: Run applications in isolated environments, ensuring they work the same across all platforms.
2. **Microservices Architecture**: Develop and deploy microservices independently, allowing modularization and scalability.
3. **Continuous Integration and Deployment (CI/CD)**: Automate and streamline software testing, building, and deployment processes.
4. **Environment Replication**: Easily replicate environments for development, testing, and production.
5. **Resource Optimization**: Use system resources efficiently by running multiple containers on a single host.

---

## What are Docker Images?

A **Docker image** is a lightweight, standalone, and executable package that includes everything needed to run an application:

- Code
- Runtime
- System libraries
- System tools
- Configuration files

Images are **read-only templates** created using a **Dockerfile** or pulled from Docker Hub (a repository for sharing Docker images). They serve as the blueprint for creating containers.

---

## What are Docker Containers?

A **Docker container** is a running instance of a Docker image. It is a lightweight, standalone, and executable environment for running applications. Containers are:

- Isolated from the host system and other containers.
- Portable and can run on any system with Docker installed.
- Ephemeral by nature (though they can be made persistent if needed).

Think of a container as the **live, running version of an image**.

---

## Key Differences Between Images and Containers

| Feature           | Docker Image                      | Docker Container                       |
| ----------------- | --------------------------------- | -------------------------------------- |
| **State**         | Static, immutable                 | Dynamic, running or stopped            |
| **Purpose**       | Blueprint/template for containers | Execution environment for applications |
| **Lifespan**      | Permanent until deleted           | Temporary but can be restarted         |
| **Modifiability** | Read-only                         | Can be modified during runtime         |

---

## What is a Dockerfile?

A **Dockerfile** is a text file that contains a series of instructions used to build a Docker image. It serves as the blueprint for creating the image and defines how an application should be set up in the container.

---

## What is in a Dockerfile?

A Dockerfile typically contains:

1. **Base Image**: Specifies the starting point for the image (e.g., `FROM ubuntu:latest`).
2. **Maintainer Information**: Declares the author of the image (e.g., `MAINTAINER yourname@example.com`).
3. **Instructions**:
   - `RUN`: Executes commands to install software and dependencies.
   - `COPY`/`ADD`: Copies files or directories into the image.
   - `CMD`/`ENTRYPOINT`: Defines the default command to run when the container starts.
   - `ENV`: Sets environment variables.
   - `EXPOSE`: Exposes a specific port for the container to communicate.
   - `WORKDIR`: Sets the working directory inside the container.

### Example Dockerfile

```dockerfile
# Use an official Python runtime as a base image
FROM python:3.9-slim

# Set the working directory inside the container
WORKDIR /app

# Copy the application files to the container
COPY . /app

# Install dependencies
RUN pip install -r requirements.txt

# Set environment variables
ENV FLASK_ENV=production

# Expose port 5000 for the application
EXPOSE 5000

# Define the command to run the application
CMD ["python", "app.py"]
```

---

# Running Docker Scripts in the `ostwald` and `random_walk` Subdirectories

This experiment is based on the paper: "[Teaching the Growth, Ripening, and Agglomeration of Nanostructures in Computer Experiments](https://pubs.acs.org/doi/10.1021/acs.jchemed.6b01008)" von D. Diesing und J. Meyburg.

This guide will walk you through setting up Docker on your system, navigating the `ostwald` and `random_walk` subdirectories, and running Docker scripts to output files into specific folders.

## Prerequisites

- Ensure you have [Docker Desktop](https://www.docker.com/products/docker-desktop/) installed on your Mac, Linux, or Windows system.
- Familiarity with terminal or command-line usage is helpful, as this guide includes terminal commands for building and running Docker containers. (Below is a short introduction to navigating the terminal)

### Step 1: Install Docker

1. **Download Docker Desktop**

   - For **Mac**: [Download Docker Desktop for Mac](https://docs.docker.com/desktop/install/mac-install/).
   - For **Linux**: [Download Docker Desktop for Linux](https://docs.docker.com/desktop/install/linux-install/).
   - For **Windows**: [Download Docker Desktop for Windows](https://docs.docker.com/desktop/install/windows-install/).
     **_[Here you can watch a Guide for installing Docker on Windows](https://www.youtube.com/watch?v=bw-bMhlhcpg)_**

2. **Follow the Installation Instructions**

3. **Verify Docker Installation**
   After installation, open a terminal (or Command Prompt/PowerShell on Windows) and run:
   ```bash
   docker --version
   ```

You should see Docker’s version information if it’s installed correctly.

### Step 2: Running Docker Scripts in `ostwald` and `random_walk`

Once Docker is installed, navigate to the directory where your dockerfile is located. We’ll be running dockerfiles in each of the `ostwald` and `random_walk` subdirectories. Each subdirectory has a Dockerfile that can be built and executed using Docker commands. Here’s how to do it.

#### OS-Specific Instructions

##### For Mac Users

1. Open a terminal and navigate to either the `ostwald` or `random_walk` folder.
2. Run the following commands:

   ```zsh
   docker build -t myprogram .
   docker run --rm -v -d $(pwd)/output:/output myprogram
   ```

##### For Windows Users

1. Open **PowerShell** (or Command Prompt) and navigate to either the `ostwald` or `random_walk` folder.

2. Use the following commands:

   - **PowerShell**:

     ```powershell
     docker build -t myprogram .
     docker run --rm -v -d ${PWD}/output:/output myprogram
     ```

   - **Command Prompt**:
     ```cmd
     docker build -t myprogram .
     docker run --rm -v -d %cd%/output:/output myprogram
     ```

   > **Note**: Ensure Docker Desktop is running before executing these commands on Windows.

   **_Here is an instruction video showing how to run the ostwald simulation._** (You can also find the video by clinking [this link](./media/running_ostwald_simulation.mp4))

   <video controls src="media/running_ostwald_simulation.mp4" title="Running Ostwald Simulation"></video>

3. The output files will be generated in the `output` folder within the subdirectory you’re working in.

> [!IMPORTANT]
> Once the container finished running, move the output folder to a different location before executing the script once more or the contents of the output folder will be overwritten.

#### Explanation of Commands

- `docker build -t myprogram .`: Builds a Docker image named `myprogram` from the Dockerfile in the current directory.
- `docker run --rm -v -d $(pwd)/output:/output myprogram`: Runs the `myprogram` container, mounts the local `output` directory into the container’s `/output` directory, and automatically removes the container after it exits.

> **Note for Windows PowerShell users**: `${PWD}` is used to represent the current directory, similar to `$(pwd)` in Linux/Mac. For Windows Command Prompt users, `%cd%` can be used as an alternative.

## How to Navigate in the Terminal

### For Mac and Linux Users

In the terminal (or command-line interface), the basic navigation commands are the same for both macOS and Linux:

1. **Changing Directories**:  
   To move into a directory, use the `cd` (change directory) command. For example, to go into the `ostwald` folder:

   ```bash
   cd ostwald
   ```

   To move back to the previous directory:

   ```bash
    cd ..
   ```

2. **Listing Files**:
   To view the files in the current directory, use the ls command:

   ```bash
    ls
   ```

3. **Current Directory**:
   To display the path of the current directory, use the pwd (print working directory) command:

   ```bash
    pwd
   ```

4. **Navigating to a Specific Directory**:
   If you want to jump to a specific directory, provide the full path or relative path from your current location:

   ```bash
   cd /path/to/Docker/ostwald
   ```

### For Windows Users

Windows users can navigate in **PowerShell** or **Command Prompt**. Here are some basic navigation commands for both environments:

1. **Changing Directories**:
   In **PowerShell** and **Command Prompt**, use the `cd` (change directory) command. To navigate into the `ostwald` folder, for example:

   ```powershell
   cd ostwald
   ```

   To move back to the previous directory:

   ```powershell
    cd ..
   ```

1. **Listing Files:**
   In **PowerShell**, use ls or dir to list files:

   ```powershell
    ls
   ```

   In **Command Prompt**, use dir:

   ```c,d
    dir
   ```

1. **Current Directory:**
   In **PowerShell**, use pwd (like in Mac/Linux) to show the current directory:

   ```powershell
    pwd
   ```

   In **Command Prompt**, use cd to display the current directory:

   ```cmd
    cd
   ```

1. **Navigating to a Specific Directory:**
   In **PowerShell** and **Command Prompt**, use the full or relative path to navigate to a specific directory:

   ```powershell
    cd C:\path\to\Docker\ostwald
   ```

## Additional Resources

### Docker

For more in-depth information, you can explore the following resources:

- [Docker Documentation](https://docs.docker.com/)
- [Docker Desktop Installation Guide](https://docs.docker.com/desktop/)
- [Docker Commands Reference](https://docs.docker.com/engine/reference/commandline/docker/)

Following these steps will allow you to successfully build and run the Docker containers in your `ostwald` and `random_walk` subdirectories. Docker will output the results into the specified folders, ensuring that all files are organized within their respective directories.

### Terminal

Learning how to navigate your terminal (or command-line interface) is essential when using Docker. Below are resources that can help you master terminal navigation on different operating systems:

- [Navigating the Terminal (Mac/Linux)](https://www.codecademy.com/learn/learn-the-command-line/modules/learn-the-command-line-navigation/cheatsheet)
- [Windows Command Line Basics](https://learn.microsoft.com/en-us/windows-server/administration/windows-commands/windows-commands)
- [Navigating the PowerShell Command Line](https://learn.microsoft.com/en-us/powershell/scripting/samples/managing-current-location?view=powershell-7.4)
