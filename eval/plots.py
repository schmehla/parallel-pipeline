import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages

INPUT_SIZES = [10000, 1000000]
STAGES_LIST = ["100", "100 100", "100 100 100 100 100 100 100 100 100 100"]
ASYMETRIC_STAGES_LIST = ["10 1000 10"]
MAX_THREADPOOL_SIZE = 12

pdf = PdfPages('plots.pdf')


static_parallel_df = pd.read_csv(f'static_parallel_timing.txt').drop(columns=['iter'])
static_parallel_df = static_parallel_df.groupby(['threadpool_size', 'batch_size', 'input_size', 'stages']).agg(['mean', 'std'])
dynamic_parallel_df = pd.read_csv(f'dynamic_parallel_timing.txt').drop(columns=['iter'])
dynamic_parallel_df = dynamic_parallel_df.groupby(['threadpool_size', 'batch_size', 'input_size', 'stages']).agg(['mean', 'std'])
naive_df = pd.read_csv(f'naive_timing.txt').drop(columns=['iter'])
naive_df = naive_df.groupby(['input_size', 'stages']).agg(['mean', 'std'])
tbb_df = pd.read_csv(f'tbb_timing.txt').drop(columns=['iter'])
tbb_df = tbb_df.groupby(['n_tokens', 'batch_size', 'input_size', 'stages']).agg(['mean', 'std'])

def batch_size_plot(stages):
    fig, (ax1, ax2) = plt.subplots(ncols=2, sharey=True)
    input_size = max(INPUT_SIZES)

    df = static_parallel_df.query(f"input_size == {input_size}").droplevel('input_size')
    df = df.query(f"stages == '{stages}'").droplevel('stages')
    df = df.query(f"batch_size != 100")
    df = df.unstack('threadpool_size')
    df_mean = df['time']['mean']
    df_std = df['time']['std']
    df_mean.plot.bar(ax=ax1, yerr=df_std, figsize=((12, 7)))
    
    df = tbb_df.query(f"input_size == {input_size}").droplevel('input_size')
    df = df.query(f"stages == '{stages}'").droplevel('stages')
    df = df.query(f"batch_size != 100")
    df = df.unstack('n_tokens')
    df_mean = df['time']['mean']
    df_std = df['time']['std']
    df_mean.plot.bar(ax=ax2, yerr=df_std, figsize=((12, 7)))
    
    ax1.grid()
    ax1.set_title('static threadpool')
    ax2.grid()
    ax2.set_title('intel tbb')
    fig.suptitle(f'Time to send input of size {input_size} through a pipeline with stages {stages}.')
    ax1.set_ylabel('Time [ms]')
    pdf.savefig()

def stages_naive_vs_parallel(stages_to_compare):
    batch_size = 10000
    input_size = max(INPUT_SIZES)

    fig, (ax1, ax2, ax3, ax4) = plt.subplots(ncols=4, sharey=True)

    df = static_parallel_df.query(f'batch_size == {batch_size}').droplevel('batch_size')
    df = df.query(f'input_size == {input_size}').droplevel('input_size')
    df = df.query('stages == @stages_to_compare')
    df = df.unstack('stages')
    df_mean = df['time']['mean']
    df_std = df['time']['std']
    df_mean.plot.bar(ax=ax1, yerr=df_std, figsize=((12, 7)))

    df = dynamic_parallel_df.query(f'batch_size == {batch_size}').droplevel('batch_size')
    df = df.query(f'input_size == {input_size}').droplevel('input_size')
    df = df.query('stages == @stages_to_compare')
    df = df.unstack('stages')
    df_mean = df['time']['mean']
    df_std = df['time']['std']
    df_mean.plot.bar(ax=ax2, yerr=df_std)

    df = tbb_df.query(f'batch_size == {batch_size}').droplevel('batch_size')
    df = df.query(f'input_size == {input_size}').droplevel('input_size')
    df = df.query('stages == @stages_to_compare')
    df = df.unstack('stages')
    df_mean = df['time']['mean']
    df_std = df['time']['std']
    df_mean.plot.bar(ax=ax3, yerr=df_std)

    df = naive_df.query(f'input_size == {input_size}').droplevel('input_size')
    df = pd.concat({'naive': df}, names=[''])
    df = df.query('stages == @stages_to_compare')
    df = df.unstack('stages')
    df_mean = df['time']['mean']
    df_std = df['time']['std']
    df_mean.plot.bar(ax=ax4, yerr=df_std)

    ax1.grid()
    ax1.set_title('static threadpool')
    ax1.get_legend().remove()
    ax2.grid()
    ax2.set_title('dynamic threadpool')
    ax2.get_legend().remove()
    ax3.grid()
    ax3.set_title('intel tbb')
    ax3.get_legend().remove()
    ax4.grid()
    ax4.set_title('naive')
    fig.suptitle(f'Time to send input of size {input_size} through a pipeline (batch size = {batch_size}).')
    ax1.set_ylabel(f"Time [ms]")
    pdf.savefig()

stages_naive_vs_parallel(STAGES_LIST[:2])
stages_naive_vs_parallel([STAGES_LIST[-1]] + ASYMETRIC_STAGES_LIST)
for stages in STAGES_LIST:
    batch_size_plot(stages)
pdf.close()