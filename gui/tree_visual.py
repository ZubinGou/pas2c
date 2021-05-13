# from ete3 import Tree, TreeStyle
from ete3 import Tree, TreeStyle

# Loads a tree structure from a newick string. The returned variable 't' is the root node for the tree.
t = Tree("(A:1,(B:1,(K:121,Fuck_U:1,D:1)Z:0.5)wo:0.5);", format=1)
# t2 = Tree("(A:1,(B:1,(E:1,D:1)Internal_1:0.5)Internal_2:0.5)Root;", format=1)

# syntax_tree = Tree("syntax_tree.nh")
circular_style = TreeStyle()
circular_style.mode = "c"  # draw tree in circular mode
circular_style.scale = 20
# t.render("mytree.png", w=183, units="mm", tree_style=circular_style)

print(t.write())
print(t.get_ascii(show_internal=True))


# t = Tree()
# t.populate(15) # We create a random tree topology
# print(t)
# print(t.children)
# print(t.get_children())
# print(t.up)
# print(t.name)
# print(t.dist)
# print(t.is_leaf())
# print(t.get_tree_root())
# print(t.children[0].get_tree_root())
# print(t.children[0].children[0].get_tree_root())

# for leaf in t:
#   print(leaf.name)
