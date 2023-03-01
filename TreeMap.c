#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TreeMap.h"

#define MAX(a, b) (((a) >= (b))?(a):(b))


/* Creeaza un arbore cu o serie de metode asociate
 *
 * return: arborele creat
 */
TTree* createTree(void* (*createElement)(void*),
				  void (*destroyElement)(void*),
				  void* (*createInfo)(void*),
				  void (*destroyInfo)(void*),
				  int compare(void*, void*)) {

	TTree* newTree = malloc(sizeof(TTree));
	newTree->root = NULL;
	newTree->size = 0;
	newTree->createElement = createElement;
	newTree->destroyElement = destroyElement;
	newTree->createInfo = createInfo;
	newTree->destroyInfo = destroyInfo;
	newTree->compare = compare;
	return newTree;
}



/* Verifica daca un arbore este gol (vid)
 * 		1 - daca arborele este gol
 * 		0 - in caz contrar
 */
int isEmpty(TTree* tree) {
	
	return (tree->root == NULL);
}


/* Cauta un anumit element in interiorul unui arbore
 *
 * tree: structura cu metodele asociate arborelui
 *   !folosit pentru a putea compara elementul curent cu cel cautat
 *
 * x: radacina arborelui curent (in care se face cautarea)
 * elem: elementul ce trebuie cautat
 */
TreeNode* search(TTree* tree, TreeNode* x, void* elem) {
	
	while (x != NULL) {
		if (tree->compare(x->elem, elem) == 0) {
			return x;
		} else if (tree->compare(x->elem, elem) == 1) {
			x = x->left;
		} else {
			x = x->right;
		}
	}
	return NULL;
}


/* Gaseste nodul cu elementul minim dintr-un arbore
 * avand radacina in x
 */
TreeNode* minimum(TreeNode* x) {
	if (x != NULL) {
		while(x->left != NULL) {
			x = x->left;
		}
		return x;
	} 
	return NULL;
}

/* Gaseste nodul cu elementul maxim dintr-un arbore
 * avand radacina in x
 */
TreeNode* maximum(TreeNode* x) {
		if (x != NULL) {
		while(x->right != NULL) {
			x = x->right;
		}
		return x;
	} 
	return NULL;
}


/* Functie pentru gasirea succesorului unui nod
 * (succesorul in inordine)
 */
TreeNode* successor(TreeNode* x) {
	TreeNode* node = x;
	
	if (node->right != NULL) {
		
		node = node->right;
		return minimum(node);
	}
	else {

		while (node->parent != NULL) {
			if (node == node->parent->left) {
				return node->parent;
			}
			node = node->parent;
		}

	}
	return NULL;
}


/* Functie pentru gasirea predecesorului unui nod
 * (predecesorul in inordine)
 */
TreeNode* predecessor(TreeNode* x) {
	TreeNode* node = x;
	if (node->left != NULL) {
		node = node->left;
		return maximum(node);
	}
	else {

		while (node->parent != NULL) {
			if (node == node->parent->right) {
				return node->parent;
			}
			node = node->parent;
		}

	}
	return NULL;
}


/* Actualizeaza inaltimea unui nod din arbore
 */
void updateHeight(TreeNode* x) {

	int leftHeight = 0;
	int rightHeight = 0;

	if (x != NULL) {
		if (x->left != NULL)  leftHeight  = x->left->height;
		if (x->right != NULL) rightHeight = x->right->height;
		x->height = MAX(leftHeight, rightHeight) + 1;
	}
}


/* Functie ce primeste adresa unui arbore si
 * a unui nod x si realizeaza o rotatie la stanga
 * a subarborelui care are varful in x
 *
 *   (x)	 Rotatie    (y)
 *	 / \     stanga     / \
 *  a  (y)    ---->   (x)  c
 *	   / \			  / \
 *    b   c          a   b
 */
void avlRotateLeft(TTree* tree, TreeNode* x) {
	if (tree == NULL || x == NULL) return;
	TreeNode* y = x->right;

	x->right = y->left;
	if (x->right != NULL) {
		x->right->parent = x;
	}
	y->left = x;

	//verificam ce fel de copil era x
	if (x->parent != NULL) {
		if(x == x->parent->right) {
			x->parent->right = y; 
		} else {
			x->parent->left = y;
		}

	} else {
		tree->root = y;
	}
	y->parent = x->parent;
	x->parent = y;

	updateHeight(x);
	updateHeight(y);/**/

}


/* Functie ce primeste adresa unui arbore si
 * a unui nod x si realizeaza o rotatie la dreapta
 * a subarborelui care are varful in y
 *
 *     (y)	 Rotatie    (x)
 *	   / \   dreapta    / \
 *   (x)  c   ---->    a  (y)
 *	 / \			      / \
 *  a   b                b   c
 */
void avlRotateRight(TTree* tree, TreeNode* y) {
	if (tree == NULL || y == NULL) return;
	TreeNode* x = y->left;

	y->left = x->right;
	if(y->left != NULL) {
		y->left->parent = y;
	}
	x->right = y;

	if (y->parent != NULL) {
		if(y == y->parent->right) {
			y->parent->right = x;
		} else {
			y->parent->left = x;
		}
	} else {
		tree->root = x;
	}
	x->parent = y->parent;
	y->parent = x;

	updateHeight(y);
	updateHeight(x);
}


/* Calculeaza factorul de echilibrare pentru un nod x
 * (AVL balance factor)
*/
int avlGetBalance(TreeNode *x) {
	if (x == NULL) {
		return 0;
	} else {
		int heightL = 0;
		int heightR = 0;
		if(x->left != NULL) heightL = x->left->height;
		if(x->right != NULL) heightR = x->right->height;

		return heightL - heightR;
	}
}


/* Functie pentru reechilibrarea unui arbore AVL
 * in urma unei inserari prin rotatii simple sau duble
 * si recalcularea inaltimii fiecarui nod intalnit parcurgand
 * arborele de jos in sus, spre radacina
 *
 */
void avlFixUp(TTree* tree, TreeNode* y) {

		int balanceY = avlGetBalance(y);

		if(balanceY < -1 || balanceY > 1) {
			//Left Left
			if (balanceY > 1 && avlGetBalance(y->left) <= 1) {
				avlRotateRight(tree, y);
			} //Right Right 
			else if (balanceY < -1 && avlGetBalance(y->right) <= -1) {
				avlRotateLeft(tree, y);
			} //Right Left
			else if (balanceY < -1 && avlGetBalance(y->right) == 1) {
				avlRotateRight(tree, y->right);
				avlRotateLeft(tree, y);
			} //Left Right
			else if (balanceY > 1 && avlGetBalance(y->left) == -1) {
				avlRotateLeft(tree, y->left);
					avlRotateRight(tree, y);				
			}
		}
}


/* Functie pentru crearea unui nod
 *
 * value: valoarea/cheia din cadrul arborelui
 * info: informatia/valoarea din dictionar
 */
TreeNode* createTreeNode(TTree *tree, void* value, void* info) {

	if (tree == NULL)
		return NULL;

	// Alocarea memoriei
	TreeNode* node = (TreeNode*) malloc(sizeof(TreeNode));

	// Setarea elementului si a informatiei
	node->elem = tree->createElement(value);
	node->info = tree->createInfo(info);

	// Initializarea legaturilor din cadrul arborelui
	node->parent = node->right = node->left = NULL;

	// Initializarea legaturilor din cadrul listei dublu inlantuite
	node->next = node->prev = node->end = NULL;


	// Inaltimea unui nod nou este 1
	// Inaltimea lui NULL este 0
	node->height = 1;

	return node;
}


/* Inserarea unul nou nod in cadrul multi-dictionarului
 * ! In urma adaugarii arborele trebuie sa fie echilibrat
 *
 */
void insert(TTree* tree, void* elem, void* info) {
	if (tree == NULL)
		return;

	TreeNode* newNode = createTreeNode(tree, elem, info);

	if (tree->root == NULL && newNode != NULL) {
		//Daca arborele era gol nou nod devine radacina
		tree->root = newNode;
		newNode->end = newNode;

	} else if (tree->root != NULL) {
		//Se cauta daca acest nod deja exista in arbore
		TreeNode* duplicate = search(tree, tree->root, elem);
		TreeNode* parent = NULL;


		if (duplicate == NULL) {
			
			TreeNode* root = tree->root;
			//Mergem iterativ prin arbore si gasim pozitia de inserat
			while (root != NULL) {
				
				if (tree->compare(elem, root->elem) == -1) {
					parent = root;
					root = root->left;
				} else {
					parent = root;
					root = root->right;
				}
			}
			//Verificam ce fel de copil este noul nod pentru parent
			if(tree->compare(elem, parent->elem) == -1) {
				parent->left = newNode;
				newNode->parent = parent;
			} else {
				parent->right = newNode;
				newNode->parent = parent;
			}
			//Adaugam in lista dublu inlantuita
			TreeNode* pred = predecessor(newNode);
			TreeNode* suc = successor(newNode);
			
			newNode->next = suc;
			
			if (pred != NULL) {
				newNode->prev = pred->end;
			} else {
				newNode->prev = NULL;
			}
			
			newNode->end = newNode;
			
			if (suc != NULL) {
				suc->prev = newNode->end;
			}
			
			if (pred != NULL) {
				pred->next = newNode;
			}

		} else {
			//daca deja exista aceasta valoare in arbore
			//adaugam nodul doar in lista dubluinlantuita
			newNode->next = duplicate->end->next;
			newNode->prev = duplicate->end;

			if (newNode->next != NULL) {
				newNode->next->prev = newNode;
			}

			duplicate->end->next = newNode;
			duplicate->end = newNode;
		}
		
		//Reechilibram arborele dupa ce am inserat
		while (newNode != NULL) {

			updateHeight(newNode); 
			avlFixUp(tree, newNode);
			newNode = newNode->parent;
		}
	} 
	tree->size++;
}


/* Eliminarea unui nod dintr-un arbore
 *
 * ! tree trebuie folosit pentru eliberarea
 *   campurilor `elem` si `info`
 * */
void destroyTreeNode(TTree *tree, TreeNode* node){

	// Verificarea argumentelor functiei
	if(tree == NULL || node == NULL) return;

	// Folosirea metodelor arborelui
	// pentru de-alocarea campurilor nodului
	tree->destroyElement(node->elem);
	tree->destroyInfo(node->info);

	// Eliberarea memoriei nodului
	free(node);
}


/* Eliminarea unui nod din arbore
 *
 * elem: cheia nodului ce trebuie sters
 * 	! In cazul in care exista chei duplicate
 *	  se va sterge ultimul nod din lista de duplicate
 */
void delete(TTree* tree, void* elem) {

	TreeNode* node = search(tree, tree->root, elem);
	TreeNode* rebalance = NULL;
	//Verificam daca nodul nu are duplicate
	if (node == node->end) {
		//Elimin din lista dublu inlantuita
		TreeNode* urmatorul = successor(node);
		TreeNode* precedent = predecessor(node);
		//Am luat pe cazuri
		if (precedent != NULL && urmatorul != NULL) {
			precedent->next = urmatorul;
			urmatorul->prev = precedent;
		} 
		else if (precedent == NULL && urmatorul != NULL) {
			urmatorul->prev = NULL;
		} 
		else if (precedent != NULL && urmatorul == NULL) {
			precedent->next = NULL;
		}

		//daca nu are nici un copil
		if (node->left == NULL && node->right == NULL) {
			if(tree->root != node) {
				//verificam ce fel de copil este pentru parintele sau
				//si punem ca parintele sa pointeze la NULL pe ramura aceea
				if (node->parent->left == node) {
					node->parent->left = NULL;
				} else {
					node->parent->right = NULL;
				}
				//Memoram de unde sa incepem sa rebalansam arborele
				if (node->parent != NULL) {
					rebalance = node->parent; 
				} else {
					rebalance = NULL;
				}
				//Pur si simplu stergem nodul
				destroyTreeNode(tree, node);
			} else {
				destroyTreeNode(tree, node);
				tree->root = NULL;
			}

		} //Are un copil in partea stanga
		else if (node->left != NULL && node->right == NULL) {

			if (node->parent != NULL) {
				node->left->parent = node->parent;
				node->parent->left = node->left;
			} else {
				tree->root = node->left;
				node->left->parent = NULL;
			}
			//Memoram de unde incepe rebalansarea
			rebalance = node->left;				
			destroyTreeNode(tree, node);
		} //Are un copil in partea dreapta
		else if (node->left == NULL && node->right != NULL) {
			if (node->parent != NULL) {
				node->right->parent = node->parent;
				node->parent->right = node->right;
			} else {
				tree->root = node->right;
				node->right->parent = NULL;
			}
			//Memoram de unde incepe rebalansarea
			rebalance = node->right;				
			destroyTreeNode(tree, node);			
		} //Are doi copii
		else if(node->left != NULL && node->right != NULL) {

			TreeNode* suc = successor(node);
			suc->left = node->left;
			node->left->parent = suc;
			if (suc != node->right) {

				suc->right = node->right;
				node->right->parent = suc;
				suc->parent->left = NULL;
				rebalance = suc->parent;
			} 
			else {
				rebalance = suc;
			}
			//Relatia cu parintii daca diff de root
			if (node != tree->root) {
				
				suc->parent = node->parent;
				
				if (node == node->parent->left) {
					node->parent->left = suc;
				} else {
					node->parent->right = suc;
				}
			} else {
				tree->root = suc;
				suc->parent = NULL;
			}
			
			destroyTreeNode(tree, node);
		}

	} //Daca avem duplicate
	else {
		//stergem nodul de la end
		node->end->prev->next = node->end->next;
		if(node->end->next != NULL) {
			node->end->next->prev = node->end->prev;
		}
		TreeNode* aux = node->end;
		node->end = aux->prev;
		destroyTreeNode(tree, aux);
	}
	//Rebalansam Arborele
	while (rebalance != NULL) {
		updateHeight(rebalance); 
		avlFixUp(tree, rebalance);
		rebalance = rebalance->parent;
	}
	
	tree->size--;

}


/* Eliberarea memoriei unui arbore
 */
void destroyTree(TTree* tree){

	/* Se poate folosi lista dublu intalntuita
	 * pentru eliberarea memoriei
	 */
	if (tree == NULL || tree->root == NULL)
		return;
	TreeNode* first = minimum(tree->root);
	while(first != NULL) {
		TreeNode* aux = first;
		first = first->next;
		destroyTreeNode(tree,aux);
	}
	free(tree);
}