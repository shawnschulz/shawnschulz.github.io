import scanpy as sc
import sqlite3
import pandas as pd 

# For now, just take an input .h5ad file and  output the top 10 most highly variable gene names

# 1. Need to check if need to do scanpy's highly variable gene calculation and do it if so
# (Later) Should use preexisting cell type annotations, and/or infer cell type annotation
# 2. Need to annotate whether the top most highly variable genes are surface receptors
# 3. Need to get the top 10 most highly variable surface receptors
# 4. Need to calculate relative % level of transcription among the most highly variable surface receptors
# 5. Need to output that information, probably just in csv for now but eventually add it to a sql DB
# (note) I think it makes more sense to calculate the color mapping in the renderer rather than do it here, but the cmap should be added to the DB

def process_h5ad(adata):
    """
    Check if need to do scanpy's highly variable gene calculation, do it if so.
    Check if need to infer cell type. For now, skip the cell type annotation if it doesn't already exist
    """
    if not adata.var:
        raise ValueError("Did not detect .var annotations in h5ad file, were genes properly aligned and annotated?")
    if "highly_variable" not in adata.var.columns:
        sc.pp.log1p(adata)
        sc.pp.highly_variable(adata)
    if "cell_type" not in adata.obs.columns:
        print("Did not detect cell type annotatoins in obs. Warning: please ensure cell type annotations are labeled as 'cell_type' in obs dataframe")
        # Code to infer cell type would go here
    return

def run_celltype_inference(embedding, X):
    """
    Runs celltpye inference for one cell. Should already have the embeding loaded into memory and call a simple 
    function call to infer the cell type based on the cell info given. Can add more options to this function later
    """ 
    pass

def annotate_genes(adata):
    """
    Use NCBI gene names to annotate whether genes are surface receptors or not, among other useful annotations.
    This construts the entire gene SQL database, with primary key as ensembl gene IDs, a different function
    should be used for accessing the generated db 
    """
    pass
