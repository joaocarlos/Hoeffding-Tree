import sklearn_json as skj

from sklearn.datasets import load_iris
from sklearn import tree
import graphviz
import numpy as np
from collections import deque
from sklearn.tree import DecisionTreeClassifier
from sklearn.tree import _tree as ctree
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle

from dtreeviz.trees import *

def main():
    print("Hello World!")
    num_classes = 43
    num_features = 6
    classes = [f'C{x}' for x in range(num_classes)]
    attributes = [f'F{x}' for x in range(num_features)]
    dataset = np.genfromtxt('train.dat', delimiter=' ', dtype=int)
    classif_array = dataset[:, num_features]

    for filename in ["out"]:
        # for filename in ["tree", "treeCopy"]:
        # for filename in ["treeCopy"]:

        clf = skj.from_json(f'{filename}.json')

        try:
            print("\n 1st GEN STARTED")
            dot_data = tree.export_graphviz(clf, out_file=None,
                                            feature_names=attributes,
                                            class_names=classes,
                                            filled=True, rounded=True,
                                            special_characters=True)
            graph = graphviz.Source(dot_data)
            graph.render(f"{filename}.dot")
            print("\n 1st GEN ENDED")
        except Exception:
            print(f"Error with {filename} in first gen.")

        try:
            print("\n 2nd GEN STARTED")
            viz = dtreeviz(clf,
                           dataset,
                           classif_array,
                           target_name='Cluster',
                           feature_names=attributes,
                           class_names=classes
                           )

            viz.save(f"{filename}.svg")
            print("\n 2nd GEN ENDED")
        except Exception:
            print(f"Error with {filename} in second gen.")



if __name__ == "__main__":
    main()