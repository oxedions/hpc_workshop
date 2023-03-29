# User environment

This practical session is focused on understanding and setting a user environment for HPC usage.

## SSH

We need a secure way to reach a remote system and interact with it (shell and data). Ssh allows secured authentication and data transfer between client an server.

### Keys

In order to ssh to a remote cluster, it is key to understand the concept of public key cryptography. Please read the concept of keys at https://en.wikipedia.org/wiki/Public-key_cryptography 
It is important to understand difference between asymmetric and symmetric cryptography.

Once understood, on your local system, generate a new ssh key pair using:

```
ssh-keygen -t ed25519
```

And press enter multiple time to answer all questions and also do not provide a passphrase for now.
The passphrase is useful later to protect your key from being easily stolen by someone.

Once generated, you should have on your local system 2 new files in your $HOME/.ssh/ folder: id_ed25519 and id_ed25519.pub. The first file is your private key, that you MUST keep secure and not shared with anyone. The second file is the public key, that can be shared. It is also the public key that we will register on the remote ssh server to be able to login without password.

Get content of the public key file, and keep it somewhere:

```
cat $HOME/.ssh/id_ed25519.pub
```

ssh on your remote system, using password, and once logged, create .ssh folder if not existing, and add key into authorized_keys file into this folder:

```
mkdir -p $HOME/.ssh/
vi $HOME/.ssh/authorized_keys  # Copy past content of id_ed25519.pub in the file at this step
chmod 600 $HOME/.ssh/authorized_keys
```

Then exit, and try to login again. You should now be able to login without password, using only your private key. You can see the authentication exchange during ssh connection by adding -vvv.

### Port forwarding

SSH allows port forwarding. Its an effective feature to "link" distant port to your local system, and be able to reach an external server (http or anything else) from your local system.
It is also an efficient way to bypass some network restrictions or start a socks 5 proxy. Please keep in mind that badly used, ssh port forwarding can lead to serious security issues!

To test this feature, on a remote system, start a web browser with a custom page, on port 8888, in a dedicated shell:

```
sudo apt-get install python3 -y
echo "Hello from my remote server!" > index.html
python3 -m http.server 8888
```

Now, open another shell on your local system, and ssh on the remote system binding your local port 8080 to the distant port 8888:

```
ssh my_user@my_server -L 8080:localhost:8888
```

And let this shell alive.

Now, on your local web browser, if you connect to your local host, on port 8080 (http://localhost:8080), you should see the web page.

This port forwarding can also be used in reverse (be extra careful regarding security when using reverse !!!) or using -D to open a socks 5 proxy (extremely useful to be used as a proxy for web browsers). Port forwarding allows to bypass many firewalls restrictions, but to be used with extreme care.

## Linux environment

It is important as parallel developer, HPC clusters users, and system administrators to master basic Linux environment. A nice training can be found here, and it should only take around 10-15 min.

https://networking.ringofsaturn.com/Unix/learnUNIXin10minutes.php

## Bash basics

Bash basics are important to know how to create simple scripts to manipulate data, automate tasks, etc.
An interesting tutorial can be found at https://www.learnshell.org . You should at least do the Basics section for now. Advanced usages can be seen later when needed.

## Vim

Vim is one of the most popular tools to edit text files on remote systems. Prefer Vim to Vi, as Vim is way easier to manipulate and use in day to day usage.

Install the tool:

```
sudo apt-get install vim -y
```

And follow the online tutorial to learn how to use basic vim: https://www.openvim.com/

Note that while the tutorial advertise usage of hjku keys to navigate, you can use standard arrows on the keyboard.

Note also that while it seem a difficult to use editor at first sight, after few usages it should become easier, and even mandatory once mastered, since the tool offer so much features.

## Scripting

Bash scripting is a mandatory knowledge to manipulate distant Linux systems.

Lets create a simple script:

Hello world

Calculator and variables

Loops

Pipes

Colors

## Modules

Modules can be loaded using Lmod tool.

Install Lmod:

```
sudo apt-get install lmod -y
```

Environment Modules are useful to load environment variables, and unload them for a specific library or software made available to users. They are also efficient at loading dependencies needed for a specific element.

Create 2 naive example tools, 1 needing the other one to run.
Create first file basic_tool_1 in folder /tmp/basic_tools/bin (you will need to create this folder) with the following content:

#!/usr/bin/env bash
echo Value is 4

And make it executable:

chmod +x /tmp/basic_tools/bin/basic_tool_1

Then create another tool, that needs the first one to run. Create folder /tmp/advanced_tools/bin and add in this folder file advanced_tool_1 with the following content:

#!/usr/bin/env bash
initial_value=$(basic_tool_1 | awk -F ' ' '{print $3}')
echo Final value is $((4 * initial_value))

## Slurm cluster

### Build Slurm

sudo apt-get install gcc make wget libmunge2 libmunge-dev munge
wget https://download.schedmd.com/slurm/slurm-23.02.0.tar.bz2
tar xvjf slurm-23.02.0.tar.bz2
cd slurm-23.02.0
./configure
make
sudo make install

