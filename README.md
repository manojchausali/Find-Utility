# Find-Utility
 Implementation for the variants of find utility of the Linux distributions.  The problem consists of three parts. Follow the description of each part to understand the  requirement and the desired output.
** Task 1: Basic find**
 In this task, you need to implement the basic find utility. The program should list all the
 f
 iles present in the root directory. The absolute path to the root directory will be provided as
 command line argument.
 Figure 1 shows the structure of a directory dir1 located in path /home/os/Documents/dir1.
 The listing 1 shows the corresponding output for the directory structure shown in the figure 1.
 Do not include any directory in your output
 <img width="467" alt="image" src="https://github.com/user-attachments/assets/0636ed34-0222-45df-b952-e52d949db8fa">
<img width="518" alt="image" src="https://github.com/user-attachments/assets/0c476695-a0f5-4f54-adc2-c1550c22f1cc">

 **Task 2: Filter out the files **
 close
 free
 In this part, you will extend the find utility from task 1. Given a size X KB and file type Z, Z
 will be a integer constant in range 0 to 7. Your program should print all files having size >= X
 and file type <= Z. Different file type available on Linux are listed in table 1. We have added
 ”UNKNOWN”to identify an unsupported file type.
 HINT: You need to create a custom-mapping from Linux file type to Integer type assigned
 to each file in table 1
 Syntax
 $./task2 <relative path to a directory> <size in KB> <file type integer>
  You can use the below mentioned APIs to implement this part of the assignment. Refer to
 man page of these APIs to know about their usage.
 pipe
 perror
 lstat
 exit
 stat
 sprintf
 close
 free
 <img width="531" alt="image" src="https://github.com/user-attachments/assets/5690160e-0392-4728-9f6c-34ed23e0278e">
Example
 Figure2showsthestructureofadirectorycalleddir1(present inthepath/home/os/Docu
ments)withsizeofeachfileinBytes
 Output
 Thelisting2listthedesiredoutputfordirectorystructureshowninthefigure2
 Listing2:Task2output
 $ gcc find task2.c−o task2
 $ ./task2 /home/os/Documents/dir1 3 4
 /home/os/Documents/dir1/dir11 4KBDIR
 /home/os/Documents/dir1/dir12 4KBDIR
 /home/os/Documents/dir1/dir12/dir21 4KBDIR
 /home/os/Documents/dir1/dir12/dir21/file23.txt 3KBREG
 /home/os/Documents/dir1/dir12/file22.txt 158KBREG
 /home/os/Documents/dir1/dir12/file21.txt 204KBREG
 $
 **Task3:Parallel find**
  Your task is to parallelize the find implementation. Given a root directory, create a child
 process to install the files with in a subdirectory in the root.You should create a child process
 for every root subdirectory(refer to the Example).The child should identify all the files in the
 subdirectory and communicate the list to the parent through pipe. The parent will read the
 list from the pipe and write it to standard output.
 Example
 The listing 3 lists the output corresponding to the directory structure in figure 1
 Listing 3: Task3 output
 $ gcc find
 task3 . c −o task3
 $ ./task3 /home/os/Documents/dir1
 /home/os/Documents/dir1/ file1 . txt
 /home/os/Documents/dir1/dir11/file11 . txt
 /home/os/Documents/dir1/dir11/file12 . txt
 /home/os/Documents/dir1/dir12/dir21/file23 . txt
 /home/os/Documents/dir1/dir12/file22 . txt
 /home/os/Documents/dir1/dir12/file21 . txt
 /home/os/Documents/dir1/ file2 . txt
 $
 The child process will be created for listing files of dir11 and dir12. During processing of
 subdirectory dir12, new child process should not be created for directory dir21
 NOTE:
 The directory structure in the task3 can contain a symbolic link. A symbolic link is a
 special type of file on Linux that points to another file or directory. Symbolic links can be
 present anywhere in the root (in this example, dir1) directory tree. You should resolve the
 symbolic links and list the name of file (if a link points to a file) and all files in the directory
 pointed by a symbolic link instead of reporting the size of the symbolic link file.
 Using pipe is mandatory for communicating the list of files in the subdirectory processed
 by the child to the parent.
 
 Assumption: A symbolic link will never point to itself recursively. The size of filepath of
 all the files in a subdirectory of relative path to a directory will be always < 2KB.
 System calls and library functions allowed- fork- free- opendir- readdir- closedir- read- write- strcpy- strcmp- ato* family- printf family- pipe
 Implementation- malloc- stat- lstat- readlink- strlen- open- close- strcat- strto* family- wait/waitpid- exit
 
