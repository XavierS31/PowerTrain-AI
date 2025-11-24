"""
Save trained ML models and scalers for use in prediction server
Trains and saves: MLP (sklearn), Random Forest, and PyTorch Neural Network
Run this to prepare all models for the prediction API
"""

import pandas as pd
import numpy as np
from sklearn.preprocessing import StandardScaler
from sklearn.neural_network import MLPClassifier
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score
import joblib
import os
import sqlite3
import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import Dataset, DataLoader

# Set random seeds for reproducibility
torch.manual_seed(42)
np.random.seed(42)

# Define paths
PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
MODELS_DIR = os.path.join(PROJECT_ROOT, 'Software', 'models')
DATA_DIR = os.path.join(PROJECT_ROOT, 'Software', 'data')
DB_DIR = os.path.join(DATA_DIR, 'database')
DB_FILE = os.path.join(DB_DIR, 'driving_behavior.db')

os.makedirs(MODELS_DIR, exist_ok=True)

# Device configuration for PyTorch
device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
print(f"Using device: {device}")

# Load the dataset from SQLite
print(f"Loading dataset from SQLite: {DB_FILE}")
if not os.path.exists(DB_FILE):
    print(f"Error: Database file not found: {DB_FILE}")
    print("Please run create_database.py first to create the database and import data.")
    exit(1)

conn = sqlite3.connect(DB_FILE)
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
y = df["Level"].values

# Train-test split
X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.2, random_state=42, stratify=y
)

# Standardize features
scaler = StandardScaler()
X_train_scaled = scaler.fit_transform(X_train)
X_test_scaled = scaler.transform(X_test)

print(f"\nTraining samples: {len(X_train)}")
print(f"Test samples: {len(X_test)}")
print(f"Features: {X_train.shape[1]}")

# PyTorch Dataset class
class PerformanceDataset(Dataset):
    def __init__(self, features, labels):
        self.features = torch.FloatTensor(features)
        self.labels = torch.LongTensor(labels)
    
    def __len__(self):
        return len(self.labels)
    
    def __getitem__(self, idx):
        return self.features[idx], self.labels[idx]

# PyTorch Neural Network model
class PerformanceNN(nn.Module):
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

# Train MLP (sklearn)
print("\n" + "=" * 60)
print("Training MLP (sklearn)")
print("=" * 60)
mlp = MLPClassifier(
    hidden_layer_sizes=(16, 8),
    activation="relu",
    solver="adam",
    max_iter=1000,
    random_state=42
)
print("Training MLP classifier...")
mlp.fit(X_train_scaled, y_train)
y_pred_mlp = mlp.predict(X_test_scaled)
accuracy_mlp = accuracy_score(y_test, y_pred_mlp)
print(f"MLP accuracy: {accuracy_mlp:.3f}")

mlp_file = os.path.join(MODELS_DIR, 'mlp_model.joblib')
scaler_file = os.path.join(MODELS_DIR, 'scaler.joblib')
joblib.dump(mlp, mlp_file)
joblib.dump(scaler, scaler_file)
print(f"MLP model saved to: {mlp_file}")
print(f"Scaler saved to: {scaler_file}")

# Train Random Forest
print("\n" + "=" * 60)
print("Training Random Forest")
print("=" * 60)
rf = RandomForestClassifier(
    n_estimators=100,
    max_depth=20,
    min_samples_split=5,
    min_samples_leaf=2,
    max_features='sqrt',
    random_state=42,
    n_jobs=-1
)
print("Training Random Forest...")
rf.fit(X_train_scaled, y_train)
y_pred_rf = rf.predict(X_test_scaled)
accuracy_rf = accuracy_score(y_test, y_pred_rf)
print(f"Random Forest accuracy: {accuracy_rf:.3f}")

rf_file = os.path.join(MODELS_DIR, 'random_forest_model.joblib')
rf_scaler_file = os.path.join(MODELS_DIR, 'random_forest_scaler.joblib')
joblib.dump(rf, rf_file)
joblib.dump(scaler, rf_scaler_file)
print(f"Random Forest model saved to: {rf_file}")
print(f"Random Forest scaler saved to: {rf_scaler_file}")

# Train PyTorch Neural Network
print("\n" + "=" * 60)
print("Training PyTorch Neural Network")
print("=" * 60)

# Convert labels to 0-9 for PyTorch
y_train_pytorch = y_train - 1
y_test_pytorch = y_test - 1

# Create datasets
train_dataset = PerformanceDataset(X_train_scaled, y_train_pytorch)
test_dataset = PerformanceDataset(X_test_scaled, y_test_pytorch)

# Create data loaders
batch_size = 32
train_loader = DataLoader(train_dataset, batch_size=batch_size, shuffle=True)
test_loader = DataLoader(test_dataset, batch_size=batch_size, shuffle=False)

# Initialize model
model = PerformanceNN(input_size=10, hidden_size=16, num_classes=10).to(device)
criterion = nn.CrossEntropyLoss()
optimizer = optim.Adam(model.parameters(), lr=0.001)

# Train model
print("Training PyTorch neural network...")
num_epochs = 100
model.train()
for epoch in range(num_epochs):
    running_loss = 0.0
    for features, labels in train_loader:
        features = features.to(device)
        labels = labels.to(device)
        
        outputs = model(features)
        loss = criterion(outputs, labels)
        
        optimizer.zero_grad()
        loss.backward()
        optimizer.step()
        
        running_loss += loss.item()
    
    if (epoch + 1) % 20 == 0:
        avg_loss = running_loss / len(train_loader)
        print(f'Epoch [{epoch+1}/{num_epochs}], Loss: {avg_loss:.4f}')

# Evaluate PyTorch model
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

y_pred_pytorch = np.array(all_preds) + 1
y_true_pytorch = np.array(all_labels) + 1
accuracy_pytorch = accuracy_score(y_true_pytorch, y_pred_pytorch)
print(f"PyTorch NN accuracy: {accuracy_pytorch:.3f}")

# Save PyTorch model and scaler
pytorch_file = os.path.join(MODELS_DIR, 'pytorch_nn_model.pth')
pytorch_scaler_file = os.path.join(MODELS_DIR, 'pytorch_scaler.joblib')
torch.save(model.state_dict(), pytorch_file)
joblib.dump(scaler, pytorch_scaler_file)
print(f"PyTorch model saved to: {pytorch_file}")
print(f"PyTorch scaler saved to: {pytorch_scaler_file}")

# Summary
print("\n" + "=" * 60)
print("Training Summary")
print("=" * 60)
print(f"MLP (sklearn) accuracy:        {accuracy_mlp:.3f}")
print(f"Random Forest accuracy:         {accuracy_rf:.3f}")
print(f"PyTorch Neural Network accuracy: {accuracy_pytorch:.3f}")
print("\nAll models are ready for use in the prediction API!")
