//Atharv Patil - 37, Adit Poojary - 41
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LEN       50
#define COND_LEN       100
#define MAX_EMERGENCY  50
#define MAX_APPOINTMENTS 5
#define TABLE_SIZE     11

typedef struct Patient {
    int id;
    char name[NAME_LEN];
    char dept[NAME_LEN];
    struct Patient *next;
} Patient;

typedef struct HashNode {
    Patient *patient;
    struct HashNode *next;
} HashNode;

typedef struct MedicalRecord {
    int id;
    char name[NAME_LEN];
    char condition[COND_LEN];
    struct MedicalRecord *prev;
    struct MedicalRecord *next;
} MedicalRecord;

typedef struct Department {
    char name[NAME_LEN];
    struct Department *left;
    struct Department *right;
} Department;

typedef struct {
    int id;
    char name[NAME_LEN];
    int priority;
} EmergencyPatient;

typedef struct {
    int id;
    char name[NAME_LEN];
} Appointment;

Patient *patientHead = NULL;
HashNode *table[TABLE_SIZE];
MedicalRecord *recHead = NULL;
MedicalRecord *recTail = NULL;
Department *deptRoot = NULL;
EmergencyPatient emergencyQueue[MAX_EMERGENCY];
int emergencyCount = 0;
Appointment appointmentQueue[MAX_APPOINTMENTS];
int front = -1, rear = -1, apptCount = 0;

int readInt(const char *prompt) {
    char buf[64];
    int value;
    while (1) {
        printf("%s", prompt);
        if (fgets(buf, sizeof(buf), stdin) == NULL)
            exit(0);
        if (sscanf(buf, "%d", &value) == 1)
            return value;
        printf("Invalid number. Try again.\n");
    }
}

void readLine(const char *prompt, char *dest, int size) {
    printf("%s", prompt);
    if (fgets(dest, size, stdin) == NULL)
        exit(0);
    dest[strcspn(dest, "\n")] = '\0';
}

int hashFunction(int id) {
    return (id < 0 ? -id : id) % TABLE_SIZE;
}

Patient *findPatient(int id) {
    for (HashNode *cur = table[hashFunction(id)]; cur != NULL; cur = cur->next)
        if (cur->patient->id == id)
            return cur->patient;
    return NULL;
}

Patient *promptPatient(void) {
    int id = readInt("Enter Patient ID: ");
    Patient *p = findPatient(id);
    if (p == NULL)
        printf("Patient ID %d is not registered.\n", id);
    return p;
}

Department *createDepartment(const char *name) {
    Department *node = (Department *)malloc(sizeof(Department));
    if (node == NULL)
        return NULL;
    strncpy(node->name, name, NAME_LEN - 1);
    node->name[NAME_LEN - 1] = '\0';
    node->left = node->right = NULL;
    return node;
}

Department *insertDepartment(Department *node, const char *name, int *added) {
    if (node == NULL) {
        Department *created = createDepartment(name);
        *added = (created != NULL);
        return created;
    }
    int cmp = strcmp(name, node->name);
    if (cmp < 0)
        node->left = insertDepartment(node->left, name, added);
    else if (cmp > 0)
        node->right = insertDepartment(node->right, name, added);
    else
        *added = 0;
    return node;
}

Department *findDepartment(Department *node, const char *name) {
    while (node != NULL) {
        int cmp = strcmp(name, node->name);
        if (cmp == 0)
            return node;
        node = (cmp < 0) ? node->left : node->right;
    }
    return NULL;
}

Department *minDepartment(Department *node) {
    while (node->left != NULL)
        node = node->left;
    return node;
}

Department *deleteDepartmentNode(Department *node, const char *name, int *removed) {
    if (node == NULL)
        return NULL;
    int cmp = strcmp(name, node->name);
    if (cmp < 0) {
        node->left = deleteDepartmentNode(node->left, name, removed);
    } else if (cmp > 0) {
        node->right = deleteDepartmentNode(node->right, name, removed);
    } else {
        *removed = 1;
        if (node->left == NULL) {
            Department *child = node->right;
            free(node);
            return child;
        }
        if (node->right == NULL) {
            Department *child = node->left;
            free(node);
            return child;
        }
        Department *succ = minDepartment(node->right);
        strcpy(node->name, succ->name);
        int dummy = 0;
        node->right = deleteDepartmentNode(node->right, succ->name, &dummy);
    }
    return node;
}

void inorderDepartments(Department *node) {
    if (node == NULL)
        return;
    inorderDepartments(node->left);
    printf("  %s\n", node->name);
    inorderDepartments(node->right);
}

void preorderDepartments(Department *node, int depth) {
    if (node == NULL)
        return;
    printf("%*s%s\n", depth * 4, "", node->name);
    preorderDepartments(node->left, depth + 1);
    preorderDepartments(node->right, depth + 1);
}

int departmentHeight(Department *node) {
    if (node == NULL)
        return 0;
    int l = departmentHeight(node->left);
    int r = departmentHeight(node->right);
    return 1 + (l > r ? l : r);
}

void freeDepartments(Department *node) {
    if (node == NULL)
        return;
    freeDepartments(node->left);
    freeDepartments(node->right);
    free(node);
}

int departmentInUse(const char *name) {
    for (Patient *cur = patientHead; cur != NULL; cur = cur->next)
        if (strcmp(cur->dept, name) == 0)
            return 1;
    return 0;
}

void addDepartment(void) {
    char name[NAME_LEN];
    int added = 0;
    readLine("Enter Department Name: ", name, NAME_LEN);
    if (name[0] == '\0') {
        printf("Name cannot be empty.\n");
        return;
    }
    deptRoot = insertDepartment(deptRoot, name, &added);
    printf(added ? "Department added.\n" : "Department already exists.\n");
}

void searchDepartment(void) {
    char name[NAME_LEN];
    readLine("Enter Department Name to search: ", name, NAME_LEN);
    printf(findDepartment(deptRoot, name) != NULL ? "Department found.\n"
                                                  : "Department not found.\n");
}

void deleteDepartment(void) {
    char name[NAME_LEN];
    int removed = 0;
    readLine("Enter Department Name to delete: ", name, NAME_LEN);
    if (departmentInUse(name)) {
        printf("Cannot delete: patients are assigned to this department.\n");
        return;
    }
    deptRoot = deleteDepartmentNode(deptRoot, name, &removed);
    printf(removed ? "Department deleted.\n" : "Department not found.\n");
}

void displayDepartments(void) {
    if (deptRoot == NULL) {
        printf("No departments available.\n");
        return;
    }
    printf("\nDepartments (alphabetical):\n");
    inorderDepartments(deptRoot);
    printf("\nHierarchy (root first, height %d):\n", departmentHeight(deptRoot));
    preorderDepartments(deptRoot, 0);
}

void registerPatient(void) {
    int id = readInt("Enter Patient ID: ");
    if (findPatient(id) != NULL) {
        printf("Patient ID already exists.\n");
        return;
    }

    char name[NAME_LEN], dept[NAME_LEN];
    readLine("Enter Patient Name: ", name, NAME_LEN);
    readLine("Enter Department: ", dept, NAME_LEN);
    if (findDepartment(deptRoot, dept) == NULL) {
        printf("Department not found. Add it first.\n");
        return;
    }

    Patient *p = (Patient *)malloc(sizeof(Patient));
    HashNode *h = (HashNode *)malloc(sizeof(HashNode));
    if (p == NULL || h == NULL) {
        free(p);
        free(h);
        printf("Memory allocation failed.\n");
        return;
    }

    p->id = id;
    strcpy(p->name, name);
    strcpy(p->dept, dept);
    p->next = patientHead;
    patientHead = p;

    int idx = hashFunction(id);
    h->patient = p;
    h->next = table[idx];
    table[idx] = h;

    printf("Patient registered successfully.\n");
}

void displayPatients(void) {
    if (patientHead == NULL) {
        printf("No patient records found.\n");
        return;
    }
    printf("\n%-6s %-20s %-20s\n", "ID", "Name", "Department");
    printf("------------------------------------------\n");
    for (Patient *cur = patientHead; cur != NULL; cur = cur->next)
        printf("%-6d %-20s %-20s\n", cur->id, cur->name, cur->dept);
}

void searchPatient(void) {
    int id = readInt("Enter Patient ID to search: ");
    Patient *p = findPatient(id);
    if (p != NULL)
        printf("Patient Found -> ID: %d | Name: %s | Department: %s (bucket %d)\n",
               p->id, p->name, p->dept, hashFunction(id));
    else
        printf("Patient with ID %d not found.\n", id);
}

void displayHashTable(void) {
    int empty = 1;
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (table[i] == NULL)
            continue;
        empty = 0;
        printf("Bucket %2d:", i);
        for (HashNode *cur = table[i]; cur != NULL; cur = cur->next)
            printf(" [%d, %s]", cur->patient->id, cur->patient->name);
        printf("\n");
    }
    if (empty)
        printf("Hash table is empty.\n");
}

int removeRecordsFor(int id) {
    int removed = 0;
    MedicalRecord *cur = recHead;
    while (cur != NULL) {
        MedicalRecord *next = cur->next;
        if (cur->id == id) {
            if (cur->prev != NULL)
                cur->prev->next = cur->next;
            else
                recHead = cur->next;
            if (cur->next != NULL)
                cur->next->prev = cur->prev;
            else
                recTail = cur->prev;
            free(cur);
            removed++;
        }
        cur = next;
    }
    return removed;
}

void deletePatient(void) {
    int id = readInt("Enter Patient ID to delete: ");
    Patient *prev = NULL, *cur = patientHead;
    while (cur != NULL && cur->id != id) {
        prev = cur;
        cur = cur->next;
    }
    if (cur == NULL) {
        printf("Patient with ID %d not found.\n", id);
        return;
    }

    if (prev == NULL)
        patientHead = cur->next;
    else
        prev->next = cur->next;

    int idx = hashFunction(id);
    HashNode *hPrev = NULL;
    for (HashNode *h = table[idx]; h != NULL; hPrev = h, h = h->next) {
        if (h->patient == cur) {
            if (hPrev == NULL)
                table[idx] = h->next;
            else
                hPrev->next = h->next;
            free(h);
            break;
        }
    }

    int recs = removeRecordsFor(id);
    free(cur);
    printf("Patient deleted (%d medical record(s) removed).\n", recs);
}

void addRecord(void) {
    Patient *p = promptPatient();
    if (p == NULL)
        return;

    MedicalRecord *node = (MedicalRecord *)malloc(sizeof(MedicalRecord));
    if (node == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    node->id = p->id;
    strcpy(node->name, p->name);
    readLine("Enter Medical Condition: ", node->condition, COND_LEN);
    node->prev = recTail;
    node->next = NULL;

    if (recTail == NULL)
        recHead = node;
    else
        recTail->next = node;
    recTail = node;
    printf("Medical record added successfully.\n");
}

void displayRecordsForward(void) {
    if (recHead == NULL) {
        printf("No medical records available.\n");
        return;
    }
    printf("\n%-6s %-20s %s\n", "ID", "Name", "Condition");
    for (MedicalRecord *cur = recHead; cur != NULL; cur = cur->next)
        printf("%-6d %-20s %s\n", cur->id, cur->name, cur->condition);
}

void displayRecordsBackward(void) {
    if (recTail == NULL) {
        printf("No medical records available.\n");
        return;
    }
    printf("\n%-6s %-20s %s\n", "ID", "Name", "Condition");
    for (MedicalRecord *cur = recTail; cur != NULL; cur = cur->prev)
        printf("%-6d %-20s %s\n", cur->id, cur->name, cur->condition);
}

void searchRecord(void) {
    int id = readInt("Enter Patient ID to search records: ");
    int found = 0;
    for (MedicalRecord *cur = recHead; cur != NULL; cur = cur->next) {
        if (cur->id == id) {
            printf("Record Found -> ID: %d | Name: %s | Condition: %s\n",
                   cur->id, cur->name, cur->condition);
            found = 1;
        }
    }
    if (!found)
        printf("No medical records found for ID %d.\n", id);
}

void deleteRecord(void) {
    int id = readInt("Enter Patient ID to delete records: ");
    int removed = removeRecordsFor(id);
    if (removed > 0)
        printf("%d medical record(s) deleted.\n", removed);
    else
        printf("No medical records found for ID %d.\n", id);
}

void freeRecords(void) {
    MedicalRecord *cur = recHead;
    while (cur != NULL) {
        MedicalRecord *next = cur->next;
        free(cur);
        cur = next;
    }
    recHead = recTail = NULL;
}

void freePatients(void) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        HashNode *h = table[i];
        while (h != NULL) {
            HashNode *next = h->next;
            free(h);
            h = next;
        }
        table[i] = NULL;
    }
    Patient *cur = patientHead;
    while (cur != NULL) {
        Patient *next = cur->next;
        free(cur);
        cur = next;
    }
    patientHead = NULL;
}

void addEmergency(void) {
    if (emergencyCount >= MAX_EMERGENCY) {
        printf("Emergency queue is full.\n");
        return;
    }
    Patient *p = promptPatient();
    if (p == NULL)
        return;

    int priority = readInt("Enter Priority (1-Critical, 2-Serious, 3-Normal): ");
    if (priority < 1 || priority > 3) {
        printf("Invalid priority entered. Defaulting to Normal (3).\n");
        priority = 3;
    }

    EmergencyPatient e;
    e.id = p->id;
    strcpy(e.name, p->name);
    e.priority = priority;
    emergencyQueue[emergencyCount++] = e;
    printf("Emergency patient added to the queue.\n");
}

void treatEmergency(void) {
    if (emergencyCount == 0) {
        printf("No patients in the emergency queue.\n");
        return;
    }
    int best = 0;
    for (int i = 1; i < emergencyCount; i++)
        if (emergencyQueue[i].priority < emergencyQueue[best].priority)
            best = i;

    printf("Treating Patient -> ID: %d, Name: %s, Priority: %d\n",
           emergencyQueue[best].id, emergencyQueue[best].name,
           emergencyQueue[best].priority);

    for (int i = best; i < emergencyCount - 1; i++)
        emergencyQueue[i] = emergencyQueue[i + 1];
    emergencyCount--;
}

void displayEmergency(void) {
    if (emergencyCount == 0) {
        printf("No patients in the emergency queue.\n");
        return;
    }
    printf("\n%-6s %-20s %s\n", "ID", "Name", "Priority");
    for (int i = 0; i < emergencyCount; i++)
        printf("%-6d %-20s %d\n", emergencyQueue[i].id,
               emergencyQueue[i].name, emergencyQueue[i].priority);
}

void addAppointment(void) {
    if (apptCount == MAX_APPOINTMENTS) {
        printf("Appointment queue is full.\n");
        return;
    }
    Patient *p = promptPatient();
    if (p == NULL)
        return;

    if (front == -1)
        front = 0;
    rear = (rear + 1) % MAX_APPOINTMENTS;
    appointmentQueue[rear].id = p->id;
    strcpy(appointmentQueue[rear].name, p->name);
    apptCount++;
    printf("Appointment added successfully.\n");
}

void serveAppointment(void) {
    if (apptCount == 0) {
        printf("Appointment queue is empty.\n");
        return;
    }
    printf("Serving Appointment -> ID: %d | Name: %s\n",
           appointmentQueue[front].id, appointmentQueue[front].name);

    front = (front + 1) % MAX_APPOINTMENTS;
    apptCount--;
    if (apptCount == 0)
        front = rear = -1;
}

void displayAppointments(void) {
    if (apptCount == 0) {
        printf("No appointments in the queue.\n");
        return;
    }
    printf("\n--- Appointment Waiting List ---\n");
    int index = front;
    for (int i = 0; i < apptCount; i++) {
        printf("%d. ID: %d | Name: %s\n", i + 1,
               appointmentQueue[index].id, appointmentQueue[index].name);
        index = (index + 1) % MAX_APPOINTMENTS;
    }
}

void printMenu(void) {
    printf("\n========== Hospital Patient Management System ==========\n");
    printf("Patients (Linked List + Hash Table)\n");
    printf("   1. Register Patient\n");
    printf("   2. Display All Patients\n");
    printf("   3. Search Patient by ID\n");
    printf("   4. Delete Patient\n");
    printf("   5. Display Hash Table\n");
    printf("Medical Records (Doubly Linked List)\n");
    printf("   6. Add Medical Record\n");
    printf("   7. Display Records (Forward)\n");
    printf("   8. Display Records (Backward)\n");
    printf("   9. Search Medical Records\n");
    printf("  10. Delete Medical Records\n");
    printf("Emergency (Priority Queue)\n");
    printf("  11. Add Emergency Patient\n");
    printf("  12. Treat Highest Priority Emergency\n");
    printf("  13. Display Emergency Queue\n");
    printf("Appointments (Circular Queue)\n");
    printf("  14. Add Appointment\n");
    printf("  15. Serve Appointment\n");
    printf("  16. Display Appointment Queue\n");
    printf("Departments (BST)\n");
    printf("  17. Add Department\n");
    printf("  18. Search Department\n");
    printf("  19. Delete Department\n");
    printf("  20. Display Departments\n");
    printf("   0. Exit\n");
}

int main(void) {
    const char *defaults[] = {"Emergency", "Cardiology", "Pediatrics",
                              "Neurology", "Orthopedics"};
    for (int i = 0; i < 5; i++) {
        int added = 0;
        deptRoot = insertDepartment(deptRoot, defaults[i], &added);
    }

    int choice;
    do {
        printMenu();
        choice = readInt("Enter your choice: ");

        switch (choice) {
            case 1:  registerPatient(); break;
            case 2:  displayPatients(); break;
            case 3:  searchPatient(); break;
            case 4:  deletePatient(); break;
            case 5:  displayHashTable(); break;
            case 6:  addRecord(); break;
            case 7:  displayRecordsForward(); break;
            case 8:  displayRecordsBackward(); break;
            case 9:  searchRecord(); break;
            case 10: deleteRecord(); break;
            case 11: addEmergency(); break;
            case 12: treatEmergency(); break;
            case 13: displayEmergency(); break;
            case 14: addAppointment(); break;
            case 15: serveAppointment(); break;
            case 16: displayAppointments(); break;
            case 17: addDepartment(); break;
            case 18: searchDepartment(); break;
            case 19: deleteDepartment(); break;
            case 20: displayDepartments(); break;
            case 0:  printf("Exiting program.\n"); break;
            default: printf("Invalid choice. Try again.\n");
        }
    } while (choice != 0);

    freeRecords();
    freePatients();
    freeDepartments(deptRoot);
    return 0;
}
