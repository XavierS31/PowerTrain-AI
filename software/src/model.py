import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.neural_network import MLPClassifier
from sklearn.metrics import accuracy_score, classification_report, confusion_matrix
import os

# Load the dataset
filepath = "../data/raw_data/driving_behavior_dataset.csv"
df = pd.read_csv(filepath)

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

# Train-test split
X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.2, random_state=42, stratify=y
)

# Standardize features
scaler = StandardScaler()
X_train_scaled = scaler.fit_transform(X_train)
X_test_scaled = scaler.transform(X_test)

# Define a 2-hidden-layer MLP
mlp = MLPClassifier(
    hidden_layer_sizes=(16, 8),  # 2 layers: 16 neurons then 8 neurons
    activation="relu",
    solver="adam",
    max_iter=1000,
    random_state=42
)

# Train
print("Training MLP classifier...")
mlp.fit(X_train_scaled, y_train)

# Predict
y_pred = mlp.predict(X_test_scaled)

# Evaluation metrics
accuracy = accuracy_score(y_test, y_pred)
report = classification_report(y_test, y_pred, digits=3)
cm = confusion_matrix(y_test, y_pred)

print(f"\nAccuracy: {accuracy:.3f}")
print("\nClassification Report:")
print(report)
print("\nConfusion Matrix:")
print(cm)

# Create results directory if it doesn't exist
os.makedirs("../models/results", exist_ok=True)

# Visualize confusion matrix with matplotlib
plt.figure(figsize=(10, 8))
sns.heatmap(cm, annot=True, fmt='d', cmap='Blues', 
            xticklabels=range(1, 11), yticklabels=range(1, 11),
            cbar_kws={'label': 'Count'})
plt.title('Confusion Matrix - MLP Classifier', fontsize=14, fontweight='bold')
plt.xlabel('Predicted Level', fontsize=12)
plt.ylabel('Actual Level', fontsize=12)
plt.tight_layout()

# Save the confusion matrix
output_path = "../models/results/confusion_matrix.png"
plt.savefig(output_path, dpi=300, bbox_inches='tight')
print(f"\nConfusion matrix saved to: {output_path}")

# Display the plot
plt.show()
