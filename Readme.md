
# Management of static fragmentation and heap files
**Collaborators:**
- Theodora Archontaki
- Eleftheria Ekatommati
  
The project involves creating and managing heap and hash files for efficient data storage and retrieval. Here are some additional details about its functions:
 
## **Heap Files**
- **Creation:** Functions to create heap files, initializing necessary metadata and structures.
- **Management:** Handling blocks within the heap file, including adding and organizing entries.
- **Insertion:** Adding new entries to the heap file, ensuring they are placed correctly within the blocks.
- **Memory Allocation:** Efficiently managing memory for storing entries and metadata.
## **Hash Files**
- **Creation:** Functions to create hash files, setting up the initial hash table and metadata.
- **Hash Function:** Implementing a hash function to determine the placement of entries within the hash table.
- **Overflow Handling:** Managing overflow blocks when the primary hash table becomes full.
- **Insertion:** Adding entries to the hash file, using the hash function to find the correct location.
## **File Operations**
- **Opening and Closing:** Functions to open and close both heap and hash files, ensuring proper resource management.
- **Entry Management:** Functions to insert, delete, and search for entries within the files.
- **Metadata Handling:** Managing metadata for both heap and hash files, including block information and entry counts.

This project aims to provide a robust system for managing data using heap and hash file structures, ensuring efficient storage, retrieval, and management of entries.
