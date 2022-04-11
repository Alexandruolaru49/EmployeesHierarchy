#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hierarchy.h"

Tree search(Tree tree, char *employee_name) {  // functia cauta si returneaza nodul din arbore care are numele "manager_name"
  int i;
  Tree aux;
  if (tree == NULL)
    return NULL;
  if (strcmp(tree->name, employee_name) == 0) {
    return tree;
  }
  for (i = 0; i < tree->direct_employees_no; i++) {
    aux = search(tree->team[i], employee_name);
    if (aux)
      return aux;
  }
  return NULL;
}

/* Adauga un angajat nou in ierarhie.
 *
 * tree: ierarhia existenta
 * employee_name: numele noului angajat
 * manager_name: numele sefului noului angajat
 *
 * return: intoarce ierarhia modificata. Daca tree si manager_name sunt NULL,
           atunci employee_name e primul om din ierarhie.
 */
Tree hire(Tree tree, char *employee_name, char *manager_name) {
  Tree NewEmployee = (Tree)malloc(sizeof(TreeNode));
  Tree aux;

  int i, j;

  NewEmployee->team = NULL;
  NewEmployee->name = NULL;

  NewEmployee->name = employee_name;
  if (tree == NULL && manager_name == NULL) {
    NewEmployee->manager = NULL;
    NewEmployee->direct_employees_no = 0;
    return NewEmployee;
  } else {
    Tree Manager = search(tree, manager_name);
    if (Manager->direct_employees_no == 0) {
      Manager->team = malloc(sizeof(Tree));
      Manager->team[0] = NewEmployee;
    } else {
      Manager->team = realloc(
          Manager->team, (Manager->direct_employees_no + 1) * sizeof(Tree));
      Manager->team[Manager->direct_employees_no] = NewEmployee;
    }
    NewEmployee->manager = Manager;
    NewEmployee->direct_employees_no = 0;
    Manager->direct_employees_no++;

    for (i = 0; i < Manager->direct_employees_no - 1; i++) {
      for (j = i + 1; j < Manager->direct_employees_no; j++) {
        if (strcmp(Manager->team[i]->name, Manager->team[j]->name) > 0) {
          aux = Manager->team[i];
          Manager->team[i] = Manager->team[j];
          Manager->team[j] = aux;
        }
      }
    }
  }

  return tree;
}

/* Sterge un angajat din ierarhie.
 *
 * tree: ierarhia existenta
 * employee_name: numele angajatului concediat
 *
 * return: intoarce ierarhia modificata.
 */
Tree fire(Tree tree, char *employee_name) {
  Tree Manager;
  Tree Fired_Employee = search(tree, employee_name);

  if (Fired_Employee != NULL)
    Manager = Fired_Employee->manager;

  Tree aux;
  int index = 0, i, j, OK = 0;

  if (Fired_Employee == NULL || Fired_Employee == tree) {
    return tree;
  } else {
    if (Fired_Employee->direct_employees_no != 0) {

      while (OK == 0 && index < Manager->direct_employees_no) {
        if (Manager->team[index]->name == employee_name) {
          OK = 1;
        } else
          index++;
      }
      for (i = index + 1; i < Manager->direct_employees_no; i++) {
        Manager->team[i - 1] = Manager->team[i];
      }
      Manager->direct_employees_no--;
      Manager->team = realloc(Manager->team, (Manager->direct_employees_no + Fired_Employee->direct_employees_no) * sizeof(Tree));
      for (i = 0; i < Fired_Employee->direct_employees_no; i++) {
        Manager->team[Manager->direct_employees_no + i] =
            Fired_Employee->team[i];
        Fired_Employee->team[i] = NULL;
        Manager->team[Manager->direct_employees_no + i]->manager = Manager;
      }
      Manager->direct_employees_no =
          Manager->direct_employees_no + Fired_Employee->direct_employees_no;

      for (i = 0; i < Manager->direct_employees_no - 1; i++) {
        for (j = i + 1; j < Manager->direct_employees_no; j++) {
          if (strcmp(Manager->team[i]->name, Manager->team[j]->name) > 0) {
            aux = Manager->team[i];
            Manager->team[i] = Manager->team[j];
            Manager->team[j] = aux;
          }
        }
      }

    } else if (Fired_Employee->direct_employees_no == 0) {
      while (index < Manager->direct_employees_no && OK == 0) {
        if (strcmp(Manager->team[index]->name, employee_name) == 0) {
          OK = 1;
        } else
          index++;
      }

      for (i = index + 1; i < Manager->direct_employees_no; i++) {
        Manager->team[i - 1] = Manager->team[i];
      }

      Manager->direct_employees_no--;
      Manager->team =
          realloc(Manager->team, (Manager->direct_employees_no) * sizeof(Tree));
    }
  }
  destroy_tree(Fired_Employee);
  return tree;
}

/* Promoveaza un angajat in ierarhie. Verifica faptul ca angajatul e cel putin
 * pe nivelul 2 pentru a putea efectua operatia.
 *
 * tree: ierarhia existenta
 * employee_name: numele noului angajat
 *
 * return: intoarce ierarhia modificata.
 */
Tree promote(Tree tree, char *employee_name) {
  Tree Promoted_Employee = search(tree, employee_name), aux;

  if (Promoted_Employee == tree)
    return tree;

  int index = 0, i, OK = 0, j;

  if (Promoted_Employee->manager != tree &&
      Promoted_Employee->manager != NULL) {
    Tree Old_Manager = Promoted_Employee->manager;
    Tree New_Manager = Old_Manager->manager;

    while (OK == 0 && index < Old_Manager->direct_employees_no) {
      if (strcmp(Promoted_Employee->name, Old_Manager->team[index]->name) == 0) {
        OK = 1;
      } else
        index++;
    }
    for (i = index + 1; i < Old_Manager->direct_employees_no; i++) {
      Old_Manager->team[i - 1] = Old_Manager->team[i];
    }
    Old_Manager->direct_employees_no--;

    if (Promoted_Employee->direct_employees_no == 0) {
      tree = hire(tree, Promoted_Employee->name, New_Manager->name);
    } else if (Promoted_Employee->direct_employees_no != 0) {
      Old_Manager->team = realloc(Old_Manager->team, (Old_Manager->direct_employees_no + Promoted_Employee->direct_employees_no) * sizeof(Tree));
      for (i = 0; i < Promoted_Employee->direct_employees_no; i++) {
        Old_Manager->team[Old_Manager->direct_employees_no + i] = Promoted_Employee->team[i];
        Promoted_Employee->team[i] = NULL;
        Old_Manager->team[Old_Manager->direct_employees_no + i]->manager = Old_Manager;
      }
      Old_Manager->direct_employees_no = Old_Manager->direct_employees_no + Promoted_Employee->direct_employees_no;

      tree = hire(tree, Promoted_Employee->name, New_Manager->name);

      for (i = 0; i < Old_Manager->direct_employees_no - 1; i++) {
        for (j = i + 1; j < Old_Manager->direct_employees_no; j++) {
          if (strcmp(Old_Manager->team[i]->name, Old_Manager->team[j]->name) > 0) {
            aux = Old_Manager->team[i];
            Old_Manager->team[i] = Old_Manager->team[j];
            Old_Manager->team[j] = aux;
          }
        }
      }
    }

    destroy_tree(Promoted_Employee);
    return tree;
  }

  else
    return tree;
}

/* Muta un angajat in ierarhie.
 *
 * tree: ierarhia existenta
 * employee_name: numele angajatului
 * new_manager_name: numele noului sef al angajatului
 *
 * return: intoarce ierarhia modificata.
 */
Tree move_employee(Tree tree, char *employee_name, char *new_manager_name) {
  Tree Employee, Manager, aux;
  Employee = search(tree, employee_name);
  Manager = Employee->manager;
  int i, j, index = 0, OK = 0;

  if (Employee == tree) {
    return tree;
  }
  if (strcmp(Employee->manager->name, new_manager_name) == 0) {
    return tree;
  }

  while (OK == 0 && index < Manager->direct_employees_no) {
    if (strcmp(employee_name, Manager->team[index]->name) == 0) {
      OK = 1;
    } else
      index++;
  }
  for (i = index + 1; i < Manager->direct_employees_no; i++) {
    Manager->team[i - 1] = Manager->team[i];
  }
  Manager->direct_employees_no--;
  Manager->team =
      realloc(Manager->team, Manager->direct_employees_no * sizeof(Tree));

  if (Employee->direct_employees_no == 0) {
    tree = hire(tree, employee_name, new_manager_name);
  } else if (Employee->direct_employees_no != 0) {
    Manager->team = realloc(Manager->team, (Manager->direct_employees_no +  Employee->direct_employees_no) * sizeof(Tree));
    for (i = 0; i < Employee->direct_employees_no; i++) {
      Manager->team[Manager->direct_employees_no + i] = Employee->team[i];
      Employee->team[i] = NULL;
      Manager->team[Manager->direct_employees_no + i]->manager = Manager;
    }
    Manager->direct_employees_no = Manager->direct_employees_no + Employee->direct_employees_no;
    tree = hire(tree, Employee->name, new_manager_name);
  }
  for (i = 0; i < Manager->direct_employees_no - 1; i++) {
    for (j = i + 1; j < Manager->direct_employees_no; j++) {
      if (strcmp(Manager->team[i]->name, Manager->team[j]->name) > 0) {
        aux = Manager->team[i];
        Manager->team[i] = Manager->team[j];
        Manager->team[j] = aux;
      }
    }
  }
  destroy_tree(Employee);
  return tree;
}

/* Muta o echipa in ierarhie.
 *
 * tree: ierarhia existenta
 * employee_name: numele angajatului din varful echipei mutate
 * new_manager_name: numele noului sef al angajatului
 *
 * return: intoarce ierarhia modificata.
 */
Tree move_team(Tree tree, char *employee_name, char *new_manager_name) {
  Tree Employee, Manager, New_Manager, aux;
  Employee = search(tree, employee_name);
  Manager = Employee->manager;
  int index = 0, i, j, OK = 0;

  while (OK == 0 && index < Manager->direct_employees_no) {
    if (strcmp(employee_name, Manager->team[index]->name) == 0) {
      OK = 1;
    } else
      index++;
  }
  for (i = index + 1; i < Manager->direct_employees_no; i++) {
    Manager->team[i - 1] = Manager->team[i];
  }
  Manager->direct_employees_no--;

  New_Manager = search(tree, new_manager_name);
  if (Employee->direct_employees_no == 0) {
    tree = hire(tree, employee_name, new_manager_name);
    destroy_tree(Employee);
  } else if (Employee->direct_employees_no != 0) {
    New_Manager->team = realloc(New_Manager->team, (New_Manager->direct_employees_no + 1) * sizeof(Tree));
    New_Manager->team[New_Manager->direct_employees_no] = Employee;
    Employee->manager = New_Manager;
    New_Manager->direct_employees_no++;

    for (i = 0; i < New_Manager->direct_employees_no - 1; i++) {
      for (j = i + 1; j < New_Manager->direct_employees_no; j++) {
        if (strcmp(New_Manager->team[i]->name, New_Manager->team[j]->name) > 0) {
          aux = New_Manager->team[i];
          New_Manager->team[i] = New_Manager->team[j];
          New_Manager->team[j] = aux;
        }
      }
    }
  }
  return tree;
}

/* Concediaza o echipa din ierarhie.
 *
 * tree: ierarhia existenta
 * employee_name: numele angajatului din varful echipei concediate
 *
 * return: intoarce ierarhia modificata.
 */
Tree fire_team(Tree tree, char *employee_name) {
  Tree Employee = search(tree, employee_name);
  Tree Manager;
  int index = 0, i, OK = 0;
  if (Employee == tree)
    return tree;

  Manager = Employee->manager;
  while (OK == 0 && index < Manager->direct_employees_no) {
    if (strcmp(employee_name, Manager->team[index]->name) == 0) {
      OK = 1;
    } else
      index++;
  }
  for (i = index + 1; i < Manager->direct_employees_no; i++) {
    Manager->team[i - 1] = Manager->team[i];
  }
  Manager->direct_employees_no--;

  Manager->team = realloc(Manager->team, Manager->direct_employees_no * sizeof(Tree));
  destroy_tree(Employee);

  return tree;
}

void Add_To_Tree(Tree tree, Tree t, char *manager_name) {
  int i;
  if (tree != NULL) {
    t = hire(t, tree->name, manager_name);
    for (i = 0; i < tree->direct_employees_no; i++) {
      Add_To_Tree(tree->team[i], t, manager_name);
    }
  }
}

/* Afiseaza toti angajatii sub conducerea unui angajat.
 *
 * f: fisierul in care se va face afisarea
 * tree: ierarhia existenta
 * employee_name: numele angajatului din varful echipei
 */
void get_employees_by_manager(FILE *f, Tree tree, char *employee_name) {
  Tree Manager = search(tree, employee_name);
  Tree t;
  int i;

  if (Manager == NULL) {
    fprintf(f, "\n");
  } else {
    t = hire(NULL, "arbore", NULL);
    Add_To_Tree(Manager, t, "arbore");
    for (i = 0; i < t->direct_employees_no; i++) {
      fprintf(f, "%s ", t->team[i]->name);
    }
    fprintf(f, "\n");
    destroy_tree(t);
  }
}

int get_level(Tree tree, char *employee_name, int level) {
  int i, aux;
  if (tree == NULL) {
    return -1;
  }
  if (tree->name == employee_name) {
    return level;
  } else {
    for (i = 0; i < tree->direct_employees_no; i++) {
      aux = get_level(tree->team[i], employee_name, level + 1);
      if (aux != -1) {
        return aux;
      }
    }
  }
  return -1;
}

void Level_Aux(Tree tree, Tree p, int level, Tree t, char *manager_name) {
  int i;
  if (p != NULL) {
    if (get_level(tree, p->name, 0) == level) {
      t = hire(t, p->name, manager_name);
    }
    for (i = 0; i < p->direct_employees_no; i++) {
      Level_Aux(tree, p->team[i], level, t, manager_name);
    }
  }
}

/* Afiseaza toti angajatii de pe un nivel din ierarhie.
 *
 * f: fisierul in care se va face afisarea
 * tree: ierarhia existenta
 * level: nivelul din ierarhie
 */
void get_employees_by_level(FILE *f, Tree tree, int level) {
  Tree t, p = tree;
  int i;
  if (level < 0) {
    fprintf(f, "\n");
  } else {
    t = hire(NULL, "arbore", NULL);
    Level_Aux(tree, p, level, t, "arbore");
    for (i = 0; i < t->direct_employees_no; i++) {
      fprintf(f, "%s ", t->team[i]->name);
    }
    fprintf(f, "\n");
    destroy_tree(t);
  }
}

void Best_Manager(Tree tree, Tree t, int *maxim) {
  int i;
  if (tree != NULL) {
    if (tree->direct_employees_no == (*maxim)) {
      t = hire(t, tree->name, "maxim");
    } else if (tree->direct_employees_no > (*maxim)) {
      (*maxim) = tree->direct_employees_no;
      t = fire_team(t, "maxim");
      t = hire(t, "maxim", "arbore");
      t = hire(t, tree->name, "maxim");
    }
    for (i = 0; i < tree->direct_employees_no; i++) {
      Best_Manager(tree->team[i], t, maxim);
    }
  }
}

/* Afiseaza angajatul cu cei mai multi oameni din echipa.
 *
 * f: fisierul in care se va face afisarea
 * tree: ierarhia existenta
 */
void get_best_manager(FILE *f, Tree tree) {
  Tree t, nod;
  t = hire(NULL, "arbore", NULL);
  t = hire(t, "non-maxim", "arbore");
  t = hire(t, "maxim", "arbore");
  int maxim = 0, i;
  Best_Manager(tree, t, &maxim);
  nod = search(t, "maxim");
  for (i = 0; i < nod->direct_employees_no; i++) {
    fprintf(f, "%s ", nod->team[i]->name);
  }
  fprintf(f, "\n");

  destroy_tree(t);
}

Tree MoveTeamFromTree1ToTree2(Tree tree1, Tree tree2, char *employee_name, char *new_manager_name) {
  Tree Employee, Manager, New_Manager, aux;
  Employee = search(tree1, employee_name);
  if (Employee == NULL)
    return tree2;

  if (Employee != tree1) {
    Manager = Employee->manager;
    int index = 0, i, j, OK = 0;

    while (OK == 0 && index < Manager->direct_employees_no) {
      if (strcmp(employee_name, Manager->team[index]->name) == 0) {
        OK = 1;
      } else
        index++;
    }

    
    for (i = index + 1; i < Manager->direct_employees_no; i++) {
      Manager->team[i - 1] = Manager->team[i];
    }
    Manager->direct_employees_no--;
    Manager->team = realloc(Manager->team, Manager->direct_employees_no * sizeof(Tree));
    

    New_Manager = search(tree2, new_manager_name);
    
    New_Manager->team = realloc(New_Manager->team, (New_Manager->direct_employees_no + 1) * sizeof(Tree));
    New_Manager->team[New_Manager->direct_employees_no] = Employee;
    Employee->manager = New_Manager;
    New_Manager->direct_employees_no++;
    for (i = 0; i < New_Manager->direct_employees_no - 1; i++) {
          for (j = i + 1; j < New_Manager->direct_employees_no; j++) {
            if (strcmp(New_Manager->team[i]->name, New_Manager->team[j]->name) > 0) {
              aux = New_Manager->team[i];
              New_Manager->team[i] = New_Manager->team[j];
              New_Manager->team[j] = aux;
            }
          }
        }
   

  } else {
    int i, j;

    New_Manager = search(tree2, new_manager_name);
    New_Manager->team = realloc(New_Manager->team, (New_Manager->direct_employees_no + 1) * sizeof(Tree));
    New_Manager->team[New_Manager->direct_employees_no] = Employee;
    Employee->manager = New_Manager;
    New_Manager->direct_employees_no++;
    for (i = 0; i < New_Manager->direct_employees_no - 1; i++) {
      for (j = i + 1; j < New_Manager->direct_employees_no; j++) {
        if (strcmp(New_Manager->team[i]->name, New_Manager->team[j]->name) > 0) {
          aux = New_Manager->team[i];
          New_Manager->team[i] = New_Manager->team[j];
          New_Manager->team[j] = aux;
        }
      }
    }
  }
  return tree2;
}

Tree fire_team_aux(Tree tree, char *employee_name) { // functie care nu si dezaloca memorie
  Tree Employee = search(tree, employee_name);
  Tree Manager;
  int index = 0, i, OK = 0;
  if (Employee == tree)
    return tree;

  Manager = Employee->manager;
  while (OK == 0 && index < Manager->direct_employees_no) {
    if (strcmp(employee_name, Manager->team[index]->name) == 0) {
      OK = 1;
    } else
      index++;
  }
  for (i = index + 1; i < Manager->direct_employees_no; i++) {
    Manager->team[i - 1] = Manager->team[i];
  }
  Manager->direct_employees_no--;
  Manager->team =
      realloc(Manager->team, Manager->direct_employees_no * sizeof(Tree));

  return tree;
}

/* Reorganizarea ierarhiei cu un alt angajat in varful ierarhiei.
 *
 * tree: ierarhia existenta
 * employee_name: numele angajatului care trece in varful ierarhiei
 */
Tree reorganize(Tree tree, char *employee_name) {
  Tree NewRoot = search(tree, employee_name);
  if (NewRoot == NULL || NewRoot == tree)
    return tree;

  Tree Manager = NewRoot->manager;
  Tree p, q, k;
  tree = fire_team_aux(tree, NewRoot->name);
  NewRoot->manager = NULL; // NewRoot - Noua radacina a arborelui reorganizat;

  p = NewRoot;
  q = Manager;

  while (q != NULL) {
    k = q->manager;
    NewRoot = MoveTeamFromTree1ToTree2(tree, NewRoot, q->name, p->name);
    p = q;
    q = k;
  }
  
  return NewRoot;
}

/* Parcurge ierarhia conform parcurgerii preordine.
 *
 * f: fisierul in care se va face afisarea
 * tree: ierarhia existenta
 */
void preorder_traversal(FILE *f, Tree tree) {
  int i;
  fprintf(f, "%s", tree->name);
  if (tree->manager != NULL) {
    fprintf(f, "-%s ", tree->manager->name);
  } else
    fprintf(f, " ");
  for (i = 0; i < tree->direct_employees_no; i++) {
    preorder_traversal(f, tree->team[i]);
  }
  if (tree->manager == NULL && i == tree->direct_employees_no) {
    fprintf(f, "\n");
  }
}

/* Elibereaza memoria alocata nodurilor din arbore
 *
 * tree: ierarhia existenta
 */
void destroy_tree(Tree tree) {
  int i;
  if (tree == NULL)
    return;

  for (i = 0; i < tree->direct_employees_no; i++) {
    destroy_tree(tree->team[i]);
  }
  free(tree->team);
  free(tree);
  return;
}