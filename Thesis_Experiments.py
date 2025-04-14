import pandas as pd
import json
import os
import matplotlib.pyplot as plt
import glob


def analyze_experiment(csv_path):
    expected_columns = ["Run Index", "Generation", "Best Fitness", "Average Fitness", "Mutation Type", "Expansion Size", "Hash Code"]
    valid_rows = []

    with open(csv_path, "r") as file:
        for line in file:
            # Split on tab or comma
            parts = [p.strip() for p in line.strip().replace(",", "\t").split("\t")]
            if len(parts) != 7:
                continue  # Skip malformed or header lines
            try:
                # Try converting numeric parts
                float(parts[2])  # Best Fitness
                float(parts[3])  # Avg Fitness
                int(parts[0])    # Run Index
                int(parts[1])    # Generation
                int(parts[5])    # Expansion Size
                valid_rows.append(parts)
            except ValueError:
                continue

    df = pd.DataFrame(valid_rows, columns=expected_columns)

    # Convert numeric columns
    df["Run Index"] = df["Run Index"].astype(int)
    df["Generation"] = df["Generation"].astype(int)
    df["Best Fitness"] = df["Best Fitness"].astype(float)
    df["Average Fitness"] = df["Average Fitness"].astype(float)
    df["Expansion Size"] = df["Expansion Size"].astype(int)

    print(df.head())
    print("\nSummary:")
    print(df.groupby("Run Index")[["Best Fitness", "Average Fitness"]].describe())

    return df  # <- return this!

# 🟢 Call it and store the result
df = analyze_experiment("C:/Users/thort/source/repos/ThesisExperiments/best_individuals_log.csv")

def plot_fitness(df):
    run_indices = df["Run Index"].unique()
    
    for run in run_indices:
        df_run = df[df["Run Index"] == run]
        plt.figure(figsize=(10, 6))
        plt.plot(df_run["Generation"], df_run["Best Fitness"], label="Best Fitness", color="gold", linewidth=2)
        plt.plot(df_run["Generation"], df_run["Average Fitness"], label="Average Fitness", color="skyblue", linestyle="--", linewidth=2)
        plt.title(f"Fitness Progression - Run {run}")
        plt.xlabel("Generation")
        plt.ylabel("Fitness")
        plt.legend()
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
        plt.show()

# Now plot!
# plot_fitness(df)
def get_final_best_fitness(df):
    last_gen_per_run = df.groupby("Run Index")["Generation"].max().reset_index()
    df_final = pd.merge(df, last_gen_per_run, on=["Run Index", "Generation"])
    return df_final

def analyze_by_mutation_type_final(df_final):
    print("\n📊 Final Generation - Mutation Type Summary:")
    summary = df_final.groupby("Mutation Type")[["Best Fitness"]].agg(["mean", "std", "min", "max"]).round(3)
    print(summary)

    print("\n🧮 Number of Runs per Mutation Type:")
    print(df_final["Mutation Type"].value_counts())


def analyze_by_mutation_type(df):
    print("\n📊 Mutation Type Summary (mean ± std):")
    grouped = df.groupby("Mutation Type")[["Best Fitness", "Average Fitness"]].agg(["mean", "std"]).round(3)
    print(grouped)

    print("\n🧮 Counts per Mutation Type:")
    print(df["Mutation Type"].value_counts())

def plot_fitness_by_mutation(df):
    mutation_types = df["Mutation Type"].unique()

    for metric in ["Best Fitness", "Average Fitness"]:
        plt.figure(figsize=(10, 6))
        for mut in mutation_types:
            df_mut = df[df["Mutation Type"] == mut]
            df_grouped = df_mut.groupby("Generation")[metric].mean()
            plt.plot(df_grouped.index, df_grouped.values, label=f"{metric} - {mut}")
        
        plt.title(f"{metric} Progression by Mutation Type")
        plt.xlabel("Generation")
        plt.ylabel(metric)
        plt.legend()
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
        plt.show()


#analyze_by_mutation_type(df)
#plot_fitness(df)





def load_all_genomes_for_run(run_index, folder_path):
    all_files = glob.glob(os.path.join(folder_path, f"genomes_run_{run_index}_*.json"))
    genome_dict = {}

    for fpath in all_files:
        with open(fpath, 'r') as file:
            try:
                data = json.load(file)
                genome_dict.update(data)  # add or overwrite by hash code
            except json.JSONDecodeError:
                print(f"⚠️ Skipped malformed file: {fpath}")
    return genome_dict

def enrich_with_genome_info(df_final, genomes):
    df_final = df_final.copy()  # avoid modifying original

    # Map each column from genome dictionary
    df_final["Genetic Code"] = df_final["Hash Code"].map(lambda h: genomes.get(h, {}).get("genetic_code", ""))
    df_final["Mutation Choice"] = df_final["Hash Code"].map(lambda h: genomes.get(h, {}).get("mutation_choice", ""))
    df_final["Mutation Chances"] = df_final["Hash Code"].map(lambda h: genomes.get(h, {}).get("mutation_chances", []))
    df_final["Rule Map"] = df_final["Hash Code"].map(lambda h: genomes.get(h, {}).get("rule_map", {}))

    return df_final

def enrich_final_with_all_runs(df_final, folder_path):
    enriched_parts = []

    for run_idx in df_final["Run Index"].unique():
        df_subset = df_final[df_final["Run Index"] == run_idx]
        genome_dict = load_all_genomes_for_run(run_idx, folder_path)

        df_enriched = enrich_with_genome_info(df_subset, genome_dict)
        enriched_parts.append(df_enriched)

    return pd.concat(enriched_parts, ignore_index=True)


genome_folder = "C:/Users/thort/source/repos/ThesisExperiments"
df_final = get_final_best_fitness(df)
df_final_enriched = enrich_final_with_all_runs(df_final, genome_folder)
print(df_final_enriched.head())

best_by_chances = df_final_enriched.groupby(df_final_enriched["Mutation Chances"].astype(str))["Best Fitness"].max()
top_5 = df_final_enriched.sort_values(by="Best Fitness", ascending=False).head(5)
print(top_5[["Best Fitness", "Expansion Size", "Mutation Choice", "Mutation Chances"]])

best_fitness_value = df_final_enriched["Best Fitness"].max()
same_fitness_count = (df_final_enriched["Best Fitness"] == best_fitness_value).sum()

print(f"🧮 Number of individuals with Best Fitness = {best_fitness_value}: {same_fitness_count}")


same_as_best = df_final_enriched[df_final_enriched["Best Fitness"] == best_fitness_value]
print(same_as_best[["Run Index", "Expansion Size", "Mutation Choice", "Mutation Chances"]])


def plot_convergence(df):
    plt.figure(figsize=(12, 7))

    run_indices = df["Run Index"].unique()
    
    for run in run_indices:
        df_run = df[df["Run Index"] == run]
        plt.plot(df_run["Generation"], df_run["Best Fitness"], label=f"Run {run}", alpha=0.7)

    plt.title("🏁 Convergence of All Runs (Best Fitness)")
    plt.xlabel("Generation")
    plt.ylabel("Best Fitness")
    plt.grid(True, alpha=0.3)
    plt.legend(title="Runs", bbox_to_anchor=(1.05, 1), loc="upper left")
    plt.tight_layout()
    plt.show()


# plot_convergence(df)


def plot_smoothed_convergence(df, window=5):
    plt.figure(figsize=(12, 7))

    run_indices = df["Run Index"].unique()
    
    for run in run_indices:
        df_run = df[df["Run Index"] == run].sort_values("Generation")
        
        # Smooth with rolling average
        smooth = df_run["Best Fitness"].rolling(window=window, min_periods=1).mean()
        plt.plot(df_run["Generation"], smooth, label=f"Run {run}", alpha=0.8)

    plt.title(f"📈 Smoothed Convergence (Best Fitness, Window = {window})")
    plt.xlabel("Generation")
    plt.ylabel("Best Fitness (Smoothed)")
    plt.grid(True, alpha=0.3)
    plt.legend(title="Runs", bbox_to_anchor=(1.05, 1), loc="upper left")
    plt.tight_layout()
    plt.show()


def get_top_10_runs(df):
    # Get the last generation for each run
    last_gens = df.groupby("Run Index")["Generation"].max().reset_index()
    df_last = pd.merge(df, last_gens, on=["Run Index", "Generation"])
    
    # Rank by final best fitness
    top_10_runs = df_last.sort_values("Best Fitness", ascending=False)["Run Index"].unique()[:10]
    return top_10_runs

def plot_top_10_convergence(df, window=5):
    top_10_runs = get_top_10_runs(df)

    plt.figure(figsize=(12, 7))
    
    for run in top_10_runs:
        df_run = df[df["Run Index"] == run].sort_values("Generation")
        smooth = df_run["Best Fitness"].rolling(window=window, min_periods=1).mean()
        plt.plot(df_run["Generation"], smooth, label=f"Run {run}", linewidth=2)


    plt.title("🏆 Smoothed Convergence of Top 10 Runs")
    plt.xlabel("Generation")
    plt.ylabel("Best Fitness (Smoothed)")
    plt.grid(True, alpha=0.3)
    plt.legend(title="Top Runs", bbox_to_anchor=(1.05, 1), loc="upper left")
    plt.tight_layout()
    plt.show()


#plot_top_10_convergence(df, window=5)


def get_top_runs_per_expansion(df, mutation_type, top_n=5):
    df_type = df[df["Mutation Type"] == mutation_type]
    
    # Get only final generation per run
    last_gen = df_type.groupby("Run Index")["Generation"].max().reset_index()
    df_final = pd.merge(df_type, last_gen, on=["Run Index", "Generation"])
    
    # Group by Expansion Size, get top N runs for each
    top_runs = []
    for size, group in df_final.groupby("Expansion Size"):
        top_group = group.sort_values("Best Fitness", ascending=False).head(top_n)
        top_runs.extend(top_group["Run Index"].tolist())
    
    return set(top_runs)  # remove duplicates


def plot_convergence_for_runs(df, runs, title, window=5):
    plt.figure(figsize=(12, 7))

    for run in runs:
        df_run = df[df["Run Index"] == run].sort_values("Generation")
        smooth = df_run["Best Fitness"].rolling(window=window, min_periods=1).mean()
        plt.plot(df_run["Generation"], smooth, label=f"Run {run}", linewidth=2, alpha=0.8)

    plt.title(title)
    plt.xlabel("Generation")
    plt.ylabel("Best Fitness (Smoothed)")
    plt.grid(True, alpha=0.3)
    plt.legend(title="Runs", bbox_to_anchor=(1.05, 1), loc="upper left")
    plt.tight_layout()
    plt.show()


# Get top runs per mutation type and expansion size
top_word_runs = get_top_runs_per_expansion(df, mutation_type="WORD", top_n=5)
top_rule_runs = get_top_runs_per_expansion(df, mutation_type="RULE", top_n=5)

# Plot convergence for each
plot_convergence_for_runs(df, top_word_runs, "📘 WORD Mutation – Top 5 per Expansion Size")
plot_convergence_for_runs(df, top_rule_runs, "📙 RULE Mutation – Top 5 per Expansion Size")

def estimate_convergence_generation(df, delta_threshold=0.001, min_consecutive=5):
    convergence_points = {}

    for run in df["Run Index"].unique():
        df_run = df[df["Run Index"] == run].sort_values("Generation")
        fitness = df_run["Best Fitness"].values
        gens = df_run["Generation"].values

        # Compute deltas between generations
        delta = abs(pd.Series(fitness).diff())
        
        # Find when improvement becomes consistently low
        low_delta = (delta < delta_threshold)
        rolling = low_delta.rolling(min_consecutive).sum()
        
        if any(rolling >= min_consecutive):
            idx = rolling[rolling >= min_consecutive].index[0]
            convergence_points[run] = gens[idx]
        else:
            convergence_points[run] = gens[-1]  # never really converged

    return convergence_points

convergence = estimate_convergence_generation(df, delta_threshold=0.001, min_consecutive=5)
avg_convergence_gen = int(sum(convergence.values()) / len(convergence))

print(f"📉 Average convergence generation: {avg_convergence_gen}")

