"""
PyTorch Neural Network for Performance Level Prediction
2 hidden layers with 16 neurons each
"""

import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import Dataset, DataLoader
import pandas as pd
import numpy as np
import sqlite3
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.metrics import accuracy_score, classification_report, confusion_matrix
import os
import matplotlib.pyplot as plt
import seaborn as sns

# Set random seeds for reproducibility
torch.manual_seed(42)
np.random.seed(42)

# Device configuration
device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
print(f"Using device: {device}")

# Data loading
def load_data():
    """Load and preprocess the dataset from SQLite"""
    db_file = "../data/database/driving_behavior.db"
    if not os.path.exists(db_file):
        print(f"Error: Database file not found: {db_file}")
        print("Please run create_database.py first to create the database and import data.")
        return None, None, None, None, None
    
    conn = sqlite3.connect(db_file)
    df = pd.read_sql_query("SELECT * FROM driving_behavior", conn)
    conn.close()
    print(f"Loaded {len(df)} records from database")
    
    # Features and target
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
    
    # Convert to 0-9 for PyTorch (class indices should start from 0)
    y = y - 1
    
    # Train-test split
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.2, random_state=42, stratify=y
    )
    
    # Standardize features
    scaler = StandardScaler()
    X_train_scaled = scaler.fit_transform(X_train)
    X_test_scaled = scaler.transform(X_test)
    
    return X_train_scaled, X_test_scaled, y_train, y_test, scaler

# Dataset class
class PerformanceDataset(Dataset):
    """Custom Dataset for performance level prediction"""
    def __init__(self, features, labels):
        self.features = torch.FloatTensor(features)
        self.labels = torch.LongTensor(labels)
    
    def __len__(self):
        return len(self.labels)
    
    def __getitem__(self, idx):
        return self.features[idx], self.labels[idx]

# Neural network model
class PerformanceNN(nn.Module):
    """
    Neural Network with 2 hidden layers, 16 neurons each
    Input: 10 features
    Hidden Layer 1: 16 neurons
    Hidden Layer 2: 16 neurons
    Output: 10 classes (levels 0-9, representing levels 1-10)
    """
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

# Training function
def train_model(model, train_loader, criterion, optimizer, num_epochs=100):
    """Train the neural network"""
    model.train()
    train_losses = []
    
    for epoch in range(num_epochs):
        running_loss = 0.0
        for features, labels in train_loader:
            features = features.to(device)
            labels = labels.to(device)
            
            # Forward pass
            outputs = model(features)
            loss = criterion(outputs, labels)
            
            # Backward pass and optimization
            optimizer.zero_grad()
            loss.backward()
            optimizer.step()
            
            running_loss += loss.item()
        
        avg_loss = running_loss / len(train_loader)
        train_losses.append(avg_loss)
        
        if (epoch + 1) % 20 == 0:
            print(f'Epoch [{epoch+1}/{num_epochs}], Loss: {avg_loss:.4f}')
    
    return train_losses

# Evaluation function
def evaluate_model(model, test_loader):
    """Evaluate the model on test data"""
    model.eval()
    all_preds = []
    all_labels = []
    
    with torch.no_grad():
        for features, labels in test_loader:
            features = features.to(device)
            labels = labels.to(device)
            
            outputs = model(features)
            _, predicted = torch.max(outputs.data, 1)
            
            all_preds.extend(predicted.cpu().numpy())
            all_labels.extend(labels.cpu().numpy())
    
    return np.array(all_preds), np.array(all_labels)

# Main training script
if __name__ == "__main__":
    print("=" * 60)
    print("PyTorch Neural Network Training")
    print("=" * 60)
    
    # Load data
    print("\n1. Loading data...")
    result = load_data()
    if result is None:
        print("Failed to load data. Exiting.")
        exit(1)
    X_train, X_test, y_train, y_test, scaler = result
    print(f"   Training samples: {len(X_train)}")
    print(f"   Test samples: {len(X_test)}")
    print(f"   Features: {X_train.shape[1]}")
    print(f"   Classes: {len(np.unique(y_train))}")
    
    # Create datasets
    train_dataset = PerformanceDataset(X_train, y_train)
    test_dataset = PerformanceDataset(X_test, y_test)
    
    # Create data loaders
    batch_size = 32
    train_loader = DataLoader(train_dataset, batch_size=batch_size, shuffle=True)
    test_loader = DataLoader(test_dataset, batch_size=batch_size, shuffle=False)
    
    # Initialize model
    print("\n2. Initializing model...")
    model = PerformanceNN(input_size=10, hidden_size=16, num_classes=10).to(device)
    print(f"   Model architecture:")
    print(f"   - Input: 10 features")
    print(f"   - Hidden Layer 1: 16 neurons (ReLU)")
    print(f"   - Hidden Layer 2: 16 neurons (ReLU)")
    print(f"   - Output: 10 classes")
    print(f"   - Dropout: 0.2")
    
    # Loss and optimizer
    criterion = nn.CrossEntropyLoss()
    optimizer = optim.Adam(model.parameters(), lr=0.001)
    
    # Train model
    print("\n3. Training model...")
    num_epochs = 100
    train_losses = train_model(model, train_loader, criterion, optimizer, num_epochs)
    
    # Evaluate model
    print("\n4. Evaluating model...")
    y_pred, y_true = evaluate_model(model, test_loader)
    
    # Convert back to 1-10 scale for reporting
    y_pred_1_10 = y_pred + 1
    y_true_1_10 = y_true + 1
    
    # Calculate metrics
    accuracy = accuracy_score(y_true_1_10, y_pred_1_10)
    report = classification_report(y_true_1_10, y_pred_1_10, digits=3)
    cm = confusion_matrix(y_true_1_10, y_pred_1_10)
    
    print(f"\nAccuracy: {accuracy:.3f}")
    print("\nClassification Report:")
    print(report)
    print("\nConfusion Matrix:")
    print(cm)
    
    # Save model and scaler
    print("\n5. Saving model...")
    os.makedirs("../models", exist_ok=True)
    torch.save(model.state_dict(), "../models/pytorch_nn_model.pth")
    
    # Save scaler using joblib (for compatibility with other scripts)
    import joblib
    joblib.dump(scaler, "../models/pytorch_scaler.joblib")
    
    print("   Model saved to: ../models/pytorch_nn_model.pth")
    print("   Scaler saved to: ../models/pytorch_scaler.joblib")
    
    # Plot training loss
    plt.figure(figsize=(10, 6))
    plt.plot(train_losses)
    plt.title('Training Loss - PyTorch Neural Network', fontsize=14, fontweight='bold')
    plt.xlabel('Epoch', fontsize=12)
    plt.ylabel('Loss', fontsize=12)
    plt.grid(True)
    plt.tight_layout()
    
    os.makedirs("../models/results", exist_ok=True)
    plt.savefig("../models/results/pytorch_training_loss.png", dpi=300, bbox_inches='tight')
    print("   Training loss plot saved to: ../models/results/pytorch_training_loss.png")
    
    # Plot confusion matrix
    plt.figure(figsize=(10, 8))
    sns.heatmap(cm, annot=True, fmt='d', cmap='Blues',
                xticklabels=range(1, 11), yticklabels=range(1, 11),
                cbar_kws={'label': 'Count'})
    plt.title('Confusion Matrix - PyTorch Neural Network', fontsize=14, fontweight='bold')
    plt.xlabel('Predicted Level', fontsize=12)
    plt.ylabel('Actual Level', fontsize=12)
    plt.tight_layout()
    plt.savefig("../models/results/pytorch_confusion_matrix.png", dpi=300, bbox_inches='tight')
    print("   Confusion matrix saved to: ../models/results/pytorch_confusion_matrix.png")
    
    print("\n" + "=" * 60)
    print("Training complete!")
    print("=" * 60)

