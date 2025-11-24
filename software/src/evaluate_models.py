"""
Comprehensive Model Evaluation Script
Evaluates all three models: MLP (sklearn), PyTorch NN, and Random Forest
Uses scikit-learn metrics for consistent comparison
"""

import torch
import torch.nn as nn
from torch.utils.data import Dataset, DataLoader
import pandas as pd
import numpy as np
import sqlite3
from sklearn.metrics import (
    accuracy_score, classification_report, confusion_matrix,
    precision_score, recall_score, f1_score,
    roc_auc_score
)
from sklearn.preprocessing import label_binarize, StandardScaler
from sklearn.model_selection import train_test_split
from sklearn.neural_network import MLPClassifier
from sklearn.ensemble import RandomForestClassifier
import joblib
import os
import matplotlib.pyplot as plt
import seaborn as sns

# Device configuration
device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
print(f"Using device: {device}")

# Dataset class for PyTorch
class PerformanceDataset(Dataset):
    """Custom Dataset for performance level prediction"""
    def __init__(self, features, labels):
        self.features = torch.FloatTensor(features)
        self.labels = torch.LongTensor(labels)
    
    def __len__(self):
        return len(self.labels)
    
    def __getitem__(self, idx):
        return self.features[idx], self.labels[idx]

# PyTorch neural network model
class PerformanceNN(nn.Module):
    """Neural Network with 2 hidden layers, 16 neurons each"""
    def __init__(self, input_size=10, hidden_size=16, num_classes=10):
        super(PerformanceNN, self).__init__()
        self.fc1 = nn.Linear(input_size, hidden_size)
        self.fc2 = nn.Linear(hidden_size, hidden_size)
        self.fc3 = nn.Linear(hidden_size, num_classes)
        self.relu = nn.ReLU()
        self.dropout = nn.Dropout(0.2)
    
    def forward(self, x):
        out = self.fc1(x)
        out = self.relu(out)
        out = self.dropout(out)
        
        out = self.fc2(out)
        out = self.relu(out)
        out = self.dropout(out)
        
        out = self.fc3(out)
        return out

# Data loading
def load_test_data():
    """Load and preprocess test data from SQLite with same split as training"""
    db_file = "../data/database/driving_behavior.db"
    if not os.path.exists(db_file):
        print(f"Error: Database file not found: {db_file}")
        print("Please run create_database.py first to create the database and import data.")
        return None, None, None, None
    
    conn = sqlite3.connect(db_file)
    df = pd.read_sql_query("SELECT * FROM driving_behavior", conn)
    conn.close()
    print(f"Loaded {len(df)} records from database")
    
    feature_cols = [
        "InternalResistance",
        "StartingInternalResistance",
        "Voltage",
        "Current",
        "Temp",
        "AccelX",
        "AccelY",
        "AccelCombined",
        "Speed",
        "Distance",
    ]
    
    X = df[feature_cols].values
    y = df["Level"].values  # multiclass labels 1-10
    
    # Use same split as training (random_state=42)
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.2, random_state=42, stratify=y
    )
    
    return X_train, X_test, y_train, y_test

# Evaluation functions
def evaluate_with_sklearn(y_true, y_pred, y_probs=None, model_name=""):
    """Comprehensive evaluation using scikit-learn metrics"""
    # Calculate metrics
    accuracy = accuracy_score(y_true, y_pred)
    precision = precision_score(y_true, y_pred, average='weighted', zero_division=0)
    recall = recall_score(y_true, y_pred, average='weighted', zero_division=0)
    f1 = f1_score(y_true, y_pred, average='weighted', zero_division=0)
    
    # Per-class metrics
    report = classification_report(y_true, y_pred, digits=3, zero_division=0)
    cm = confusion_matrix(y_true, y_pred)
    
    # ROC AUC (one-vs-rest for multiclass) if probabilities provided
    roc_auc = None
    if y_probs is not None:
        try:
            y_true_binarized = label_binarize(y_true - 1, classes=range(10))  # Convert to 0-9
            roc_auc = roc_auc_score(y_true_binarized, y_probs, average='weighted', multi_class='ovr')
        except Exception as e:
            print(f"   Warning: Could not calculate ROC AUC for {model_name}: {e}")
    
    return {
        'accuracy': accuracy,
        'precision': precision,
        'recall': recall,
        'f1_score': f1,
        'roc_auc': roc_auc,
        'classification_report': report,
        'confusion_matrix': cm
    }

# Model evaluation functions
def evaluate_mlp(X_test, y_test):
    """Evaluate sklearn MLP model"""
    print("\n" + "=" * 60)
    print("Evaluating MLP (sklearn)")
    print("=" * 60)
    
    model_path = "../models/mlp_model.joblib"
    scaler_path = "../models/scaler.joblib"
    
    if not os.path.exists(model_path):
        print(f"   Error: Model file not found: {model_path}")
        print("   Please run save_model.py first to train and save the MLP model.")
        return None
    
    if not os.path.exists(scaler_path):
        print(f"   Error: Scaler file not found: {scaler_path}")
        return None
    
    # Load model and scaler
    print("   Loading model and scaler...")
    model = joblib.load(model_path)
    scaler = joblib.load(scaler_path)
    
    # Scale test data
    X_test_scaled = scaler.transform(X_test)
    
    # Predict
    print("   Making predictions...")
    y_pred = model.predict(X_test_scaled)
    y_probs = model.predict_proba(X_test_scaled)
    
    # Evaluate
    metrics = evaluate_with_sklearn(y_test, y_pred, y_probs, "MLP")
    
    return metrics

def evaluate_pytorch_nn(X_test, y_test):
    """Evaluate PyTorch Neural Network model"""
    print("\n" + "=" * 60)
    print("Evaluating PyTorch Neural Network")
    print("=" * 60)
    
    model_path = "../models/pytorch_nn_model.pth"
    scaler_path = "../models/pytorch_scaler.joblib"
    
    if not os.path.exists(model_path):
        print(f"   Error: Model file not found: {model_path}")
        print("   Please run train_pytorch_nn.py first to train the model.")
        return None
    
    if not os.path.exists(scaler_path):
        print(f"   Error: Scaler file not found: {scaler_path}")
        return None
    
    # Load model and scaler
    print("   Loading model and scaler...")
    scaler = joblib.load(scaler_path)
    model = PerformanceNN(input_size=10, hidden_size=16, num_classes=10).to(device)
    model.load_state_dict(torch.load(model_path, map_location=device))
    model.eval()
    
    # Scale test data
    X_test_scaled = scaler.transform(X_test)
    
    # Convert to 0-9 for PyTorch
    y_test_pytorch = y_test - 1
    
    # Create data loader
    test_dataset = PerformanceDataset(X_test_scaled, y_test_pytorch)
    test_loader = DataLoader(test_dataset, batch_size=32, shuffle=False)
    
    # Predict
    print("   Making predictions...")
    all_preds = []
    all_probs = []
    
    with torch.no_grad():
        for features, _ in test_loader:
            features = features.to(device)
            outputs = model(features)
            probs = torch.softmax(outputs, dim=1)
            _, predicted = torch.max(outputs.data, 1)
            
            all_preds.extend(predicted.cpu().numpy())
            all_probs.extend(probs.cpu().numpy())
    
    # Convert back to 1-10 scale
    y_pred = np.array(all_preds) + 1
    y_probs = np.array(all_probs)
    
    # Evaluate
    metrics = evaluate_with_sklearn(y_test, y_pred, y_probs, "PyTorch NN")
    
    return metrics

def evaluate_random_forest(X_test, y_test):
    """Evaluate Random Forest model"""
    print("\n" + "=" * 60)
    print("Evaluating Random Forest")
    print("=" * 60)
    
    model_path = "../models/random_forest_model.joblib"
    scaler_path = "../models/random_forest_scaler.joblib"
    
    if not os.path.exists(model_path):
        print(f"   Error: Model file not found: {model_path}")
        print("   Please run train_random_forest.py first to train the model.")
        return None
    
    if not os.path.exists(scaler_path):
        print(f"   Error: Scaler file not found: {scaler_path}")
        return None
    
    # Load model and scaler
    print("   Loading model and scaler...")
    model = joblib.load(model_path)
    scaler = joblib.load(scaler_path)
    
    # Scale test data
    X_test_scaled = scaler.transform(X_test)
    
    # Predict
    print("   Making predictions...")
    y_pred = model.predict(X_test_scaled)
    y_probs = model.predict_proba(X_test_scaled)
    
    # Evaluate
    metrics = evaluate_with_sklearn(y_test, y_pred, y_probs, "Random Forest")
    
    return metrics

# Plotting functions
def plot_confusion_matrix(cm, model_name, save_path):
    """Plot confusion matrix"""
    plt.figure(figsize=(10, 8))
    sns.heatmap(cm, annot=True, fmt='d', cmap='Blues',
                xticklabels=range(1, 11), yticklabels=range(1, 11),
                cbar_kws={'label': 'Count'})
    plt.title(f'Confusion Matrix - {model_name}', fontsize=14, fontweight='bold')
    plt.xlabel('Predicted Level', fontsize=12)
    plt.ylabel('Actual Level', fontsize=12)
    plt.tight_layout()
    plt.savefig(save_path, dpi=300, bbox_inches='tight')
    print(f"   Confusion matrix saved to: {save_path}")

def plot_model_comparison(results):
    """Plot comparison of all models"""
    models = []
    accuracies = []
    precisions = []
    recalls = []
    f1_scores = []
    
    for model_name, metrics in results.items():
        if metrics is not None:
            models.append(model_name)
            accuracies.append(metrics['accuracy'])
            precisions.append(metrics['precision'])
            recalls.append(metrics['recall'])
            f1_scores.append(metrics['f1_score'])
    
    if not models:
        print("   No models to compare.")
        return
    
    x = np.arange(len(models))
    width = 0.2
    
    fig, ax = plt.subplots(figsize=(12, 6))
    ax.bar(x - 1.5*width, accuracies, width, label='Accuracy', alpha=0.8)
    ax.bar(x - 0.5*width, precisions, width, label='Precision', alpha=0.8)
    ax.bar(x + 0.5*width, recalls, width, label='Recall', alpha=0.8)
    ax.bar(x + 1.5*width, f1_scores, width, label='F1-Score', alpha=0.8)
    
    ax.set_xlabel('Model', fontsize=12)
    ax.set_ylabel('Score', fontsize=12)
    ax.set_title('Model Comparison - All Metrics', fontsize=14, fontweight='bold')
    ax.set_xticks(x)
    ax.set_xticklabels(models)
    ax.legend()
    ax.set_ylim([0, 1])
    ax.grid(axis='y', alpha=0.3)
    plt.tight_layout()
    
    save_path = "../models/results/model_comparison.png"
    plt.savefig(save_path, dpi=300, bbox_inches='tight')
    print(f"   Model comparison plot saved to: {save_path}")

# Main evaluation script
if __name__ == "__main__":
    print("=" * 60)
    print("Comprehensive Model Evaluation")
    print("Evaluating: MLP (sklearn), PyTorch NN, Random Forest")
    print("=" * 60)
    
    # Load test data
    print("\nLoading test data...")
    result = load_test_data()
    if result is None:
        print("Failed to load test data. Exiting.")
        exit(1)
    X_train, X_test, y_train, y_test = result
    print(f"   Test samples: {len(X_test)}")
    
    # Evaluate all models
    results = {}
    
    # Evaluate MLP
    results['MLP (sklearn)'] = evaluate_mlp(X_test, y_test)
    
    # Evaluate PyTorch NN
    results['PyTorch NN'] = evaluate_pytorch_nn(X_test, y_test)
    
    # Evaluate Random Forest
    results['Random Forest'] = evaluate_random_forest(X_test, y_test)
    
    # Print comparison
    print("\n" + "=" * 60)
    print("MODEL COMPARISON")
    print("=" * 60)
    
    comparison_data = []
    for model_name, metrics in results.items():
        if metrics is not None:
            comparison_data.append({
                'Model': model_name,
                'Accuracy': f"{metrics['accuracy']:.4f}",
                'Precision': f"{metrics['precision']:.4f}",
                'Recall': f"{metrics['recall']:.4f}",
                'F1-Score': f"{metrics['f1_score']:.4f}",
                'ROC AUC': f"{metrics['roc_auc']:.4f}" if metrics['roc_auc'] else "N/A"
            })
    
    if comparison_data:
        df_comparison = pd.DataFrame(comparison_data)
        print("\n" + df_comparison.to_string(index=False))
        
        # Print detailed results for each model
        for model_name, metrics in results.items():
            if metrics is not None:
                print(f"\n{model_name} - Detailed Results:")
                print(f"  Accuracy:  {metrics['accuracy']:.4f}")
                print(f"  Precision: {metrics['precision']:.4f}")
                print(f"  Recall:    {metrics['recall']:.4f}")
                print(f"  F1-Score:  {metrics['f1_score']:.4f}")
                if metrics['roc_auc']:
                    print(f"  ROC AUC:   {metrics['roc_auc']:.4f}")
    else:
        print("\nNo models were successfully evaluated.")
        print("Please ensure all models have been trained first.")
    
    # Save plots
    print("\n" + "=" * 60)
    print("Generating plots...")
    os.makedirs("../models/results", exist_ok=True)
    
    # Confusion matrices for each model
    for model_name, metrics in results.items():
        if metrics is not None:
            model_safe_name = model_name.replace(' ', '_').replace('(', '').replace(')', '').lower()
            plot_confusion_matrix(
                metrics['confusion_matrix'],
                model_name,
                f"../models/results/{model_safe_name}_confusion_matrix.png"
            )
    
    # Model comparison plot
    plot_model_comparison(results)
    
    # Save comprehensive report
    print("\nSaving evaluation report...")
    report_path = "../models/results/comprehensive_evaluation_report.txt"
    with open(report_path, 'w') as f:
        f.write("=" * 60 + "\n")
        f.write("Comprehensive Model Evaluation Report\n")
        f.write("=" * 60 + "\n\n")
        
        if comparison_data:
            f.write("MODEL COMPARISON\n")
            f.write("-" * 60 + "\n")
            f.write(df_comparison.to_string(index=False))
            f.write("\n\n")
        
        for model_name, metrics in results.items():
            if metrics is not None:
                f.write("=" * 60 + "\n")
                f.write(f"{model_name} - Detailed Results\n")
                f.write("=" * 60 + "\n\n")
                f.write(f"Accuracy:  {metrics['accuracy']:.4f}\n")
                f.write(f"Precision: {metrics['precision']:.4f}\n")
                f.write(f"Recall:    {metrics['recall']:.4f}\n")
                f.write(f"F1-Score:  {metrics['f1_score']:.4f}\n")
                if metrics['roc_auc']:
                    f.write(f"ROC AUC:   {metrics['roc_auc']:.4f}\n")
                f.write(f"\nClassification Report:\n")
                f.write(metrics['classification_report'])
                f.write(f"\n\nConfusion Matrix:\n")
                f.write(str(metrics['confusion_matrix']))
                f.write("\n\n")
    
    print(f"   Comprehensive report saved to: {report_path}")
    
    print("\n" + "=" * 60)
    print("Evaluation complete!")
    print("=" * 60)

