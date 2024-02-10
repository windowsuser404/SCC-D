from sys import argv
import networkx as nx

global graph

def build_graph_from_initial_file(initial_file_path):
    global graph
    graph = nx.DiGraph()
    with open(initial_file_path, 'r') as file:
        file.readline()
        for line in file:
            node1, node2 = map(int, line.strip().split())
            graph.add_edge(node1, node2)
    return graph

def update_graph_from_operations(operations_file_path):
    global graph
    with open(operations_file_path, 'r') as file:
        for line in file:
            operation, node1, node2 = line.strip().split()
            node1, node2 = int(node1), int(node2)
            if operation == 'a':
                graph.add_edge(node1, node2)
            elif operation == 'd':
                if graph.has_edge(node1, node2):
                    graph.remove_edge(node1, node2)
    return graph

def write_to_a_file(file):
    global graph
    with open(file, 'w') as file:
        scc_list = nx.strongly_connected_components(graph)
        for scc in scc_list:
            file.write(' '.join(map(str, scc)) + '\n')


# Example usage:
initial_file_path = argv[1]  # Replace with the path to your initial file
operations_file_path = argv[2]  # Replace with the path to your operations file
output_file_path = argv[3] 

print("creating graph from file",initial_file_path)
graph = build_graph_from_initial_file(initial_file_path)
print("Initial Graph Nodes:", graph.nodes)
print("Initial Graph Edges:", graph.edges)

print("updating graph from file",operations_file_path)
graph = update_graph_from_operations(operations_file_path)
print("\nUpdated Graph Nodes:", graph.nodes)
print("Updated Graph Edges:", graph.edges)

print("outputting graph to file",output_file_path)
write_to_a_file(output_file_path)
