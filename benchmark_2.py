from sys import exec_prefix
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap
import pandas as pd
import time
import subprocess
import re
import seaborn as sns
from scipy.ndimage.filters import gaussian_filter1d

ALGO = {
    1: 'Constrained Dijkstra',
    2: 'Constrained Bellman-Ford'
}
TIME_UNIT_µS = '$\mu \mathrm{s}$'
TIME_UNIT_NS = '$\mathrm{ns}$'
FILE_NAME_TEMPLATE = 'rcsp{:d}.txt'

NUM_REPEAT = 50
NUM_VERTICES = 30
DELTA = 10
DPI = 1000

df = pd.DataFrame(
    columns=['algorithm', 'source', 'target', 'time', 'preprocess_time', 'computation_time', 'dist', 'num_vertices'])

def generate_cmd(data_file: str, algo_num: int):
    return './main data/{data} < text_input.txt'.format(data=data_file)


def excecute(cmd: str):
    result = subprocess.check_output(cmd, shell=True, timeout=None)
    return result


def post_process(res: str, i, j, algo: str):
    output = res.decode(encoding='utf-8')

    num_vertices = len(re.findall(r'Path: ([\d ]+)', output)[0].split())
    algo_deltas = list(
        map(float,
            re.findall(r'Times \(in ns\): ([\d ]+)', output)[0].split()))

    algo_preprocess_deltas = list(
        map(
            float,
            re.findall(r'Preprocess Times \(in ns\): ([\d ]+)',
                       output)[0].split()))

    algo_computation_deltas = list(
        map(
            float,
            re.findall(r'Computation Times \(in ns\): ([\d ]+)',
                       output)[0].split()))
    #algo_avg_delta = float(re.findall(r'Average time: ([\d.]+)', output)[0])
    #algo_avg_preprocess = float(re.findall(r'Average preprocess time: ([\d.]+)', output)[0])
    #algo_avg_computation = float(re.findall(r'Average computation time: ([\d.]+)', output)[0])
    dist = int(re.findall(r'Length: (\d+)', output)[0])
    for deltas, preprocess_deltas, computation_deltas in zip(algo_deltas, algo_preprocess_deltas, algo_computation_deltas):
        df.loc[len(df)] = [algo, i, j, deltas, preprocess_deltas, computation_deltas, dist, num_vertices]


def generate_chart(dataframe):
    df1 = dataframe  #.groupby(['algorithm', 'dist']).agg({'time': 'mean'}).reset_index()

    ### computation_time figure
    plt.figure(figsize=(15, 7))
    #plt.ylim(ymin=0, ymax=50)
    sns.lineplot(
        data=df1,
        x='dist',
        y='computation_time',
        hue='algorithm',
        palette=['green', 'red', 'greenyellow',
                 'darkorange'],  #sns.color_palette('RdYlGn_r')
        style='algorithm',
        markers=True,
        dashes=False,
        errorbar=('ci', 95))
    plt.yscale('log')
    plt.title('Average computation time (in ' + TIME_UNIT_NS +
              ') of the algorithms (with $95\%$ confident intervals)',
              fontweight='bold')
    plt.legend(title='Algorithms',
               loc='lower right',
               title_fontproperties={'weight': 'bold'},
               labels=[
                   ALGO[1], '_hidden', ALGO[2]
               ])
    plt.xlabel('Path weight', fontweight='bold')
    plt.ylabel('Computation time (in ' + TIME_UNIT_NS + ')', fontweight='bold')
    plt.tight_layout()
    plt.savefig('benchmark_plot_computation_task_2.pdf',
                bbox_inches='tight',
                pad_inches=0.0,
                dpi=DPI)
    


    #df1 = df1.melt(id_vars = 'algorithm')
    plt.figure(figsize=(15, 7))
    #plt.ylim(ymin=0, ymax=50)
    sns.lineplot(
        data=df1,
        x='dist',
        y='time',
        hue='algorithm',
        palette=['green', 'red', 'greenyellow',
                 'darkorange'],  #sns.color_palette('RdYlGn_r')
        style='algorithm',
        markers=True,
        dashes=False,
        errorbar=('ci', 95))
    plt.yscale('log')
    plt.title('Average total running time (in ' + TIME_UNIT_NS +
              ') of the algorithms (with $95\%$ confident intervals)',
              fontweight='bold')
    plt.legend(title='Algorithms',
               loc='lower right',
               title_fontproperties={'weight': 'bold'},
               labels=[
                   ALGO[1], '_hidden', ALGO[2]
               ])
    plt.xlabel('Path weight', fontweight='bold')
    plt.ylabel('Execution time (in ' + TIME_UNIT_NS + ')', fontweight='bold')
    plt.tight_layout()
    plt.savefig('benchmark_plot_total_task_2.pdf',
                bbox_inches='tight',
                pad_inches=0.0,
                dpi=DPI)
    
    ### preprocess_time figure
    plt.figure(figsize=(15, 7))
    #plt.ylim(ymin=0, ymax=50)
    sns.lineplot(
        data=df1,
        x='dist',
        y='preprocess_time',
        hue='algorithm',
        palette=['green', 'red', 'greenyellow',
                 'darkorange'],  #sns.color_palette('RdYlGn_r')
        style='algorithm',
        markers=True,
        dashes=False,
        errorbar=('ci', 95))
    plt.yscale('log')
    plt.title('Average preprocessing time (in ' + TIME_UNIT_NS +
              ') of the algorithms (with $95\%$ confident intervals)',
              fontweight='bold')
    plt.legend(title='Algorithms',
               loc='lower right',
               title_fontproperties={'weight': 'bold'},
               labels=[
                   ALGO[1], '_hidden', ALGO[2]
               ])
    plt.xlabel('Path weight', fontweight='bold')
    plt.ylabel('Preprocessing time (in ' + TIME_UNIT_NS + ')', fontweight='bold')
    plt.tight_layout()
    plt.savefig('benchmark_plot_preprocess_task_2.pdf',
                bbox_inches='tight',
                pad_inches=0.0,
                dpi=DPI)


    


def generate_heatmap_total(dataframe):
    for i in ALGO:
        cmap = LinearSegmentedColormap.from_list(
            "custom_map",
            ["#0CD79F", "#EBE539", "#EBB839", "#DBA316", "#F0496E", "#DA123E"])
        df1 = dataframe[dataframe['algorithm'] == ALGO[i]]
        df1 = df1.groupby([
            'algorithm', 'dist', 'source', 'target', 'num_vertices'
        ]).agg({
            'time': 'mean',
            'preprocess_time': 'mean',
            'computation_time': 'mean',
        }).reset_index()
        #df1 = df1.astype({'time': 'float'})
        df1['time'] = df1['time'] / 1000  # Convert ns to µs
        plt.figure(figsize=(15, 7))
        heatmap = sns.heatmap(data=df1.pivot(index='source',
                                             columns='target',
                                             values='time'),
                              vmin=0.0,
                              vmax=300.0,
                              cmap=cmap,
                              cbar=True,
                              linewidths=1,
                              linecolor='w',
                              square=True,
                              cbar_kws={
                                  'shrink': 1.0,
                                  'drawedges': False,
                                  'extend': 'neither',
                                  'location': 'right',
                                  'label':
                                  "Running time (in " + TIME_UNIT_μS + ")",
                                  'ticks': [0, 50, 100, 150, 200, 250, 300],
                              })
        heatmap.collections[0].colorbar.ax.set_yticklabels(
            list(
                map(lambda x: x + TIME_UNIT_μS, [
                    '$\leqslant$ 0', '50', '100', '150', '200', '250',
                    '$\geqslant$ 300'
                ])))
        heatmap.set_facecolor("#808080")
        plt.title('Heatmap of the total running time (in ' + TIME_UNIT_μS + ') of ' +
                  ALGO[i],
                  fontweight='bold')
        plt.xticks(rotation=45)
        plt.yticks(rotation=0)
        plt.xlabel('Target vertex', fontweight='bold')
        plt.ylabel('Source vertex', fontweight='bold')
        plt.tight_layout()
        plt.savefig('benchmark_total_task_2_' + str(i) + '.pdf',
                    bbox_inches='tight',
                    pad_inches=0.0,
                    dpi=DPI)
        

def generate_heatmap_preprocess(dataframe):
    for i in ALGO:
        cmap = LinearSegmentedColormap.from_list(
            "custom_map",
            ["#0CD79F", "#EBE539", "#EBB839", "#DBA316", "#F0496E", "#DA123E"])
        df1 = dataframe[dataframe['algorithm'] == ALGO[i]]
        df1 = df1.groupby([
            'algorithm', 'dist', 'source', 'target', 'num_vertices'
        ]).agg({
            'time': 'mean',
            'preprocess_time': 'mean',
            'computation_time': 'mean'
        }).reset_index()
        #df1 = df1.astype({'time': 'float'})
        df1['preprocess_time'] = df1['preprocess_time'] / 1000  # Convert ns to µs
        plt.figure(figsize=(15, 7))
        heatmap = sns.heatmap(data=df1.pivot(index='source',
                                             columns='target',
                                             values='preprocess_time'),
                              vmin=0.0,
                              vmax=300.0,
                              cmap=cmap,
                              cbar=True,
                              linewidths=1,
                              linecolor='w',
                              square=True,
                              cbar_kws={
                                  'shrink': 1.0,
                                  'drawedges': False,
                                  'extend': 'neither',
                                  'location': 'right',
                                  'label':
                                  "Running time (in " + TIME_UNIT_μS + ")",
                                  'ticks': [0, 50, 100, 150, 200, 250, 300],
                              })
        heatmap.collections[0].colorbar.ax.set_yticklabels(
            list(
                map(lambda x: x + TIME_UNIT_μS, [
                    '$\leqslant$ 0', '50', '100', '150', '200', '250',
                    '$\geqslant$ 300'
                ])))
        heatmap.set_facecolor("#808080")
        plt.title('Heatmap of the preprocessing time (in ' + TIME_UNIT_μS + ') of ' +
                  ALGO[i],
                  fontweight='bold')
        plt.xticks(rotation=45)
        plt.yticks(rotation=0)
        plt.xlabel('Target vertex', fontweight='bold')
        plt.ylabel('Source vertex', fontweight='bold')
        plt.tight_layout()
        plt.savefig('benchmark_preprocess_task_2_' + str(i) + '.pdf',
                    bbox_inches='tight',
                    pad_inches=0.0,
                    dpi=DPI)
        

def generate_heatmap_computation(dataframe):
    for i in ALGO:
        cmap = LinearSegmentedColormap.from_list(
            "custom_map",
            ["#0CD79F", "#EBE539", "#EBB839", "#DBA316", "#F0496E", "#DA123E"])
        df1 = dataframe[dataframe['algorithm'] == ALGO[i]]
        df1 = df1.groupby([
            'algorithm', 'dist', 'source', 'target', 'num_vertices'
        ]).agg({
            'time': 'mean',
            'preprocess_time': 'mean',
            'computation_time': 'mean'
        }).reset_index()
        #df1 = df1.astype({'time': 'float'})
        df1['computation_time'] = df1['computation_time'] / 1000  # Convert ns to µs
        plt.figure(figsize=(15, 7))
        heatmap = sns.heatmap(data=df1.pivot(index='source',
                                             columns='target',
                                             values='computation_time'),
                              vmin=0.0,
                              vmax=300.0,
                              cmap=cmap,
                              cbar=True,
                              linewidths=1,
                              linecolor='w',
                              square=True,
                              cbar_kws={
                                  'shrink': 1.0,
                                  'drawedges': False,
                                  'extend': 'neither',
                                  'location': 'right',
                                  'label':
                                  "Running time (in " + TIME_UNIT_μS + ")",
                                  'ticks': [0, 50, 100, 150, 200, 250, 300],
                              })
        heatmap.collections[0].colorbar.ax.set_yticklabels(
            list(
                map(lambda x: x + TIME_UNIT_μS, [
                    '$\leqslant$ 0', '50', '100', '150', '200', '250',
                    '$\geqslant$ 300'
                ])))
        heatmap.set_facecolor("#808080")
        plt.title('Heatmap of the computation time (in ' + TIME_UNIT_μS + ') of ' +
                  ALGO[i],
                  fontweight='bold')
        plt.xticks(rotation=45)
        plt.yticks(rotation=0)
        plt.xlabel('Target vertex', fontweight='bold')
        plt.ylabel('Source vertex', fontweight='bold')
        plt.tight_layout()
        plt.savefig('benchmark_computation_task_2_' + str(i) + '.pdf',
                    bbox_inches='tight',
                    pad_inches=0.0,
                    dpi=DPI)


def main():
    for file_num in [1]:  #, 2, 3, 4, 9, 10, 11, 12, 17, 18, 19, 20]:
        n = 0
        file_name = FILE_NAME_TEMPLATE.format(file_num)
        with open('data/' + file_name, mode='r', encoding='utf-8') as f:
            n = int(f.readline().split()[0])
        vertices = list(
            sorted(
                np.random.choice(range(1, n + 1),
                                 size=NUM_VERTICES,
                                 replace=False)))
        for algo_num, algo in ALGO.items():
            for i in vertices:
                for j in vertices:
                    if i != j:
                        print(i, j, file_name, algo_num)
                        list_to_write = [2, i, j, NUM_REPEAT, algo_num]

                        with open('text_input.txt',
                                  mode='w+',
                                  encoding='utf-8') as f:
                            f.write('\n'.join(map(str, list_to_write)))

                        cmd = generate_cmd(file_name, algo_num)
                        post_process(excecute(cmd), i, j, algo)
    df.to_csv('benchmark_task_2.csv', index=False)


#main()
df1 = pd.read_csv('benchmark_task_2.csv')
#generate_chart(df1)
generate_heatmap_total(df1)
generate_heatmap_preprocess(df1)
generate_heatmap_computation(df1)
