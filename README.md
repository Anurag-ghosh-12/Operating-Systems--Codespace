# Operating System Exploration in C 🔧

Welcome to **Operating System Exploration in C** — a deep dive into the inner workings of Linux, system calls, memory management, and process control. This repository is a step-by-step journey designed to bridge the gap between theory and hands-on practice, empowering you to understand how operating systems interact with hardware and manage resources.

---

## 🔢 What's Inside?

This repository is organized into **4 Days** of exploration, each building upon the previous day's knowledge. Whether you're a beginner curious about OS fundamentals or an advanced user wanting to dig deeper into system-level programming, this is the perfect place to start!

### **Day 1 👨‍🔧 Basic Linux Commands**

Master the foundation of Linux with essential commands that are the backbone of system operations. This section covers:

- Navigating the Linux filesystem
- File and directory manipulation (creating, moving, deleting)
- File permissions and ownership
- Process management (using `ps`, `top`, `kill`)
- Redirection, piping, and using wildcards

> **Outcome:** By the end of Day 1, you'll feel confident navigating Linux environments and using the terminal like a pro.

---

### **Day 2 🚀 Custom Master Boot Record (MBR)**

Uncover the secrets of the boot process by writing your very own **Master Boot Record (MBR)** in C. Dive into low-level programming and understand how your computer starts up before the OS even loads.

- Write custom bootloader code
- Understand memory segmentation and addressing
- Experiment with bootable USB drives
- Learn how BIOS hands control to the MBR

> **Outcome:** You'll have a solid grasp of how bootloaders function and will have created a custom MBR that you can run and modify.

---

### **Day 3 🧑‍💻 Process Management with `fork()` and `execve()`**

Explore **process creation and execution** using two of the most powerful system calls in Linux: `fork()` and `execve()`. You'll learn how modern operating systems handle multitasking and process control.

- Understand parent and child processes
- Create zombie and orphan processes
- Replace process images using `execve()`
- Implement inter-process communication basics

> **Outcome:** By the end of Day 3, you'll know how to manage and control processes, spawn child processes, and replace process execution flows.

---

### **Day 4 🚜 Shared Memory and Advanced `execve()` Usage**

Step into **advanced inter-process communication (IPC)** with **Shared Memory** and enhance your understanding of `execve()`.

- Create and manage shared memory segments
- Synchronize data between multiple processes
- Deep dive into `execve()` and environment variables
- Implement real-world IPC scenarios

> **Outcome:** You'll be able to write efficient C programs that share data across processes and understand the intricacies of process execution.

---

## 🔧 How to Get Started

1. **Clone the repository:**

```bash
git clone https://github.com/yourusername/os-exploration.git
cd os-exploration
```

2. **Navigate by Day:**

```bash
cd "day 1"
```

3. **Compile and Run:**

Each day's folder contains well-documented C programs. Compile them using `gcc`:

```bash
gcc program.c -o program
./program
```

---

## 💡 Why This Repo?

This repository isn't just about writing code; it's about **understanding** how things work under the hood. From basic commands to writing custom bootloaders and managing processes, you'll gain hands-on experience that demystifies the complexity of operating systems.

---

## 📢 Contributing

Feel free to contribute by submitting issues, suggesting enhancements, or opening pull requests. Let's make this an even richer resource for OS enthusiasts! I would also love to collaborate on better documentation of this repository.

---

## 📆 Roadmap

- ✅ Day 1: Basic Linux Commands
- ✅ Day 2: Custom MBR
- ✅ Day 3: `fork()` and `execve()`
- ✅ Day 4: Shared Memory & Advanced `execve()`
- ⏳ **Day 5 Coming Soon:** Signals and Interrupt Handling

---

## 🚀 Let's Get Started!

> "Learning is not attained by chance, it must be sought for with ardor and attended to with diligence." — **Abigail Adams**

Dive into the fascinating world of operating systems and uncover the magic that powers our machines!

---

**Happy Exploring!** 🌌

Feel free to explore these snippets and modify them as you learn!
