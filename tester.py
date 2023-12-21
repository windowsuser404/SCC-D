import sys
import random
import networkx as nx

graph = nx.DiGraph()
def generate_random_edges_and_write_file(filename, num_vertices, num_edges):
   # edges = set()
    edges = 0
    with open(filename, 'w') as file:
        #file.write(f"{num_vertices} {num_edges}\n")
        while (edges < num_edges):
            src = random.randint(0, num_vertices-1)
            dest = random.randint(0, num_vertices-1)
            edges+=1
            file.write(f"a {src} {dest}\n")

# Function to find SCC using NetworkX
def find_scc(filename):

    # Read the generated graph from the file
    with open(filename, 'r') as file:
        lines = file.readlines()[1:]
        for line in lines:
            src, dest = map(int, line.split())
            graph.add_edge(src, dest)

    # Find SCC using Kosaraju's algorithm
    scc = list(nx.strongly_connected_components(graph))

    print("Strongly Connected Components:")
    for component in scc:
        print(component)

# Example usage
num_vertices, num_edges = 10000000, 80000000
num_vertices = num_vertices-1
print(f"doing for {sys.argv[1]}")
percent = int(sys.argv[1])
num_edges = (num_edges//10)*percent
output_filename = "test.update"

generate_random_edges_and_write_file(output_filename, num_vertices, num_edges)
#find_scc(output_filename)

