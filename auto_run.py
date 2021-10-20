import subprocess
from parse import search
import sys
#Returns stdout from a command.
#[command, args, go, here]
def run_command(comm):
    output = subprocess.run(comm, stdout=subprocess.PIPE)
    return output.stdout.decode('utf-8')

def get_executable(algo, openmpOrCilk, stepfunction):
    return "%s/randomwalk_%s_step_function_%s" % (algo, openmpOrCilk, stepfunction)

parallels = ['cilk', 'openmp']
step_functions = ['basic', 'binary_search', 'binary_search_extra', 'histogram', 'sorted']
algorithms = ['rw-vec-parallel-condense', 'rw-hashmap-condensed', 'rw-bfs', 'rw-bfs-per-node']
graph_base_dir = "/data/commit/graphit/ajaybr/scratch/graph-dataset/clean_general/gapbs/"
datasets_base = ["roadNet-CA.weighted.mtx.wsg", "soc-orkut.mtx.wsg", "road_usa.weighted.mtx.wsg", "soc-twitter-2010.mtx.wsg", "soc-sinaweibo.mtx.wsg"]
datasets = [graph_base_dir + data for data in datasets_base]
pretty_print_names = {a:b[:-8] for a,b in zip(datasets, datasets_base)}

def runTrial(trials, algo, parallel, step, dataset, starting_points, walks, walk_length):
    totalTime = 0
    binary = get_executable(algo, parallel, step) 
    for i in range(trials):
        #print([binary, dataset, str(starting_points), str(walks), str(walk_length)])
        data = run_command([binary, dataset, str(starting_points), str(walks), str(walk_length)])
        runtime = search("Runtime: {:f}", data)
        try:
            totalTime += runtime[0]
        except:
            print("ERROR! %s returned the output:\n%s" % (binary, data))
    return totalTime / trials

def max_openmp_cilk_trial(trials, algo, step, dataset, starting_points, walks, walk_length):
    best_time = min([runTrial(trials, algo, parallel, step, dataset, starting_points, walks, walk_length) for parallel in parallels])
    return best_time

def runtest(algos, steps, data, starting_points, walks, walk_length, row_var, trials=1):
    paramSets = [algos, steps, data, starting_points, walks, walk_length]
    results = {}
    for a in paramSets[0]:
        for b in paramSets[1]:
            for c in paramSets[2]:
                for d in paramSets[3]:
                    for e in paramSets[4]:
                        for f in paramSets[5]:
                            results[(a,b,c,d,e,f)] = max_openmp_cilk_trial(trials, a,b,c,d,e,f)
                            print(str((a,b,pretty_print_names[c],d,e,f)) +" = " + str(results[(a,b,c,d,e,f)]))
    #Print things with 1 variable as the row, rest as columns
    non_row_vars = [i for i in range(6) if i!=row_var]
    columns = []
    for a in paramSets[non_row_vars[0]]:
        for b in paramSets[non_row_vars[1]]:
            for c in paramSets[non_row_vars[2]]:
                for d in paramSets[non_row_vars[3]]:
                    for e in paramSets[non_row_vars[4]]:
                        columns.append((a,b,c,d,e))
    paramLabels = ['Algorithm', 'Step function', 'Dataset', 'num points', 'num walks', 'walk length']
    #I had to change the tuple printing to not use commas
    label_row = ", ".join([paramLabels[row_var]] + [" ".join([pretty_print_names[i] if i in pretty_print_names else str(i) for i in c]) for c in columns])
    print(label_row)
    for row in paramSets[row_var]:
        s = str(row)
        for other_params in columns:
            param_tuple = tuple(other_params[:row_var] + (row,) + other_params[row_var:])
            s += ', '+str(results[param_tuple])
        print(s)

#print(max_openmp_cilk_trial(algorithms[0], step_functions[0], datasets[0], 1000, 1000, 6))
if len(sys.argv) > 1:
    if sys.argv[1]=='basic':
        #Does all algos and steps, with a road and social network.
        runtest(algorithms, step_functions, [datasets[2], datasets[3]], [1000], [1000], [6], 1)
    elif sys.argv[1]=='starts':
        dataset_id = int(sys.argv[2])
        #Tries increasingly large point sets. Skips the basic step function
        runtest(algorithms, step_functions[2:4], [datasets[dataset_id]], [1000, 2000, 4000, 8000, 16000, 32000, 64000, 128000, 256000], [1000], [6], 3)
    elif sys.argv[1]=='walks':
        dataset_id = int(sys.argv[2])
        #Tries increasingly large walk numbers. Skips the basic step function
        runtest(algorithms, step_functions[2:4], [datasets[dataset_id]], [1000], [1000, 2000, 4000, 8000, 16000, 32000, 64000, 128000, 256000], [6], 4)
    elif sys.argv[1]=='length':
        dataset_id = int(sys.argv[2])
        #Tries increasingly large walk numbers. Skips the basic step function
        #runtest(algorithms, step_functions[2:4], [datasets[2], datasets[3]], [1000], [1000], [2, 3, 4, 6, 8, 10, 15, 20, 30, 40, 50, 80, 100], 5)
        runtest(algorithms, step_functions[2:4], [datasets[dataset_id]], [1000], [1000], [2, 3, 4, 6, 8, 10, 15, 20, 30, 40, 50, 80, 100, 200, 400, 800], 5)