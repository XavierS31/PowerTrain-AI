"""
Random Forest Classifier for Performance Level Prediction
Using scikit-learn
"""

import pandas as pd
import numpy as np
import sqlite3
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import accuracy_score, classification_report, confusion_matrix
import joblib
import os
import matplotlib.pyplot as plt
import seaborn as sns

# Set random seed for reproducibility
np.random.seed(42)

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
    
    # Train-test split
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.2, random_state=42, stratify=y
    )
    
    # Standardize features
    scaler = StandardScaler()
    X_train_scaled = scaler.fit_transform(X_train)
    X_test_scaled = scaler.transform(X_test)
    
    return X_train_scaled, X_test_scaled, y_train, y_test, scaler

# Main training script
if __name__ == "__main__":
    print("=" * 60)
    print("Random Forest Classifier Training")
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
    
    # Initialize Random Forest
    print("\n2. Initializing Random Forest...")
    rf = RandomForestClassifier(
        n_estimators=100,        # Number of trees
        max_depth=20,            # Maximum depth of trees
        min_samples_split=5,     # Minimum samples to split a node
        min_samples_leaf=2,      # Minimum samples in a leaf
        max_features='sqrt',      # Number of features to consider for best split
        random_state=42,
        n_jobs=-1                # Use all available cores
    )
    
    print(f"   Model parameters:")
    print(f"   - Number of trees: 100")
    print(f"   - Max depth: 20")
    print(f"   - Min samples split: 5")
    print(f"   - Min samples leaf: 2")
    print(f"   - Max features: sqrt")
    
    # Train model
    print("\n3. Training Random Forest...")
    rf.fit(X_train, y_train)
    print("   Training complete!")
    
    # Predict
    print("\n4. Making predictions...")
    y_pred = rf.predict(X_test)
    
    # Calculate metrics
    accuracy = accuracy_score(y_test, y_pred)
    report = classification_report(y_test, y_pred, digits=3)
    cm = confusion_matrix(y_test, y_pred)
    
    print(f"\nAccuracy: {accuracy:.3f}")
    print("\nClassification Report:")
    print(report)
    print("\nConfusion Matrix:")
    print(cm)
    
    # Feature importance
    print("\n5. Feature Importance:")
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
    feature_importance = pd.DataFrame({
        'feature': feature_cols,
        'importance': rf.feature_importances_
    }).sort_values('importance', ascending=False)
    
    print(feature_importance.to_string(index=False))
    
    # Save model and scaler
    print("\n6. Saving model...")
    os.makedirs("../models", exist_ok=True)
    joblib.dump(rf, "../models/random_forest_model.joblib")
    joblib.dump(scaler, "../models/random_forest_scaler.joblib")
    
    print("   Model saved to: ../models/random_forest_model.joblib")
    print("   Scaler saved to: ../models/random_forest_scaler.joblib")
    
    # Plot feature importance
    plt.figure(figsize=(10, 6))
    plt.barh(feature_importance['feature'], feature_importance['importance'])
    plt.xlabel('Importance', fontsize=12)
    plt.ylabel('Feature', fontsize=12)
    plt.title('Feature Importance - Random Forest', fontsize=14, fontweight='bold')
    plt.tight_layout()
    
    os.makedirs("../models/results", exist_ok=True)
    plt.savefig("../models/results/random_forest_feature_importance.png", dpi=300, bbox_inches='tight')
    print("   Feature importance plot saved to: ../models/results/random_forest_feature_importance.png")
    
    # Plot confusion matrix
    plt.figure(figsize=(10, 8))
    sns.heatmap(cm, annot=True, fmt='d', cmap='Greens',
                xticklabels=range(1, 11), yticklabels=range(1, 11),
                cbar_kws={'label': 'Count'})
    plt.title('Confusion Matrix - Random Forest', fontsize=14, fontweight='bold')
    plt.xlabel('Predicted Level', fontsize=12)
    plt.ylabel('Actual Level', fontsize=12)
    plt.tight_layout()
    plt.savefig("../models/results/random_forest_confusion_matrix.png", dpi=300, bbox_inches='tight')
    print("   Confusion matrix saved to: ../models/results/random_forest_confusion_matrix.png")
    
    print("\n" + "=" * 60)
    print("Training complete!")
    print("=" * 60)

