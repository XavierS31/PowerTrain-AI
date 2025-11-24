# PowerTrain AI - Software Module

Machine learning system for predicting performance levels from ESP32 sensor data.

## Project Structure

```
Software/
├── data/
│   ├── raw_data/              # Raw sensor data from ESP32
│   │   └── driving_behavior_dataset.csv
│   ├── processed/             # Preprocessed data ready for ML
│   ├── database/              # SQLite database
│   │   └── driving_behavior.db
│   └── data_receiver.py       # Flask server to receive ESP32 data
│
├── models/                    # Trained models and results
│   ├── mlp_model.joblib      # MLP (sklearn) model
│   ├── scaler.joblib         # Feature scaler for MLP
│   ├── random_forest_model.joblib
│   ├── random_forest_scaler.joblib
│   ├── pytorch_nn_model.pth  # PyTorch neural network
│   ├── pytorch_scaler.joblib
│   └── results/               # Evaluation plots and reports
│
└── src/
    ├── create_database.py     # Create SQLite database from CSV
    ├── preprocessing.py       # Process raw data for ML training
    ├── save_model.py          # Train and save all three models
    ├── train_pytorch_nn.py    # Train PyTorch neural network
    ├── train_random_forest.py # Train Random Forest
    ├── train_mlp.py           # Train MLP (sklearn)
    ├── model.py               # Original MLP training script
    └── evaluate_models.py    # Evaluate all models
```

## Installation

Install required dependencies:

```bash
pip install pandas numpy scikit-learn torch matplotlib seaborn joblib flask python-dotenv
```

## Database Setup

1. Create SQLite database and import data:

```bash
cd Software/src
python create_database.py
```

This creates `Software/data/database/driving_behavior.db` with the schema and imports data from CSV.

## Model Training

### Train All Models

Train all three models (MLP, PyTorch NN, Random Forest) at once:

```bash
cd Software/src
python save_model.py
```

### Train Individual Models

Train models individually:

```bash
# MLP (sklearn)
python train_mlp.py

# PyTorch Neural Network
python train_pytorch_nn.py

# Random Forest
python train_random_forest.py
```

All models read from the SQLite database at `Software/data/database/driving_behavior.db`.

## Model Evaluation

Evaluate all trained models and compare performance:

```bash
cd Software/src
python evaluate_models.py
```

This generates:
- Confusion matrices for each model
- Model comparison plots
- Comprehensive evaluation report

## Data Preprocessing

Process raw sensor data for ML training:

```bash
cd Software/src
python preprocessing.py --input sensor_data.csv --format csv
```

## Data Receiver Server

Start the Flask server to receive sensor data from ESP32:

```bash
cd Software/data
python data_receiver.py
```

The server provides:
- `/api/data` - POST endpoint to receive sensor data
- `/api/predict` - POST endpoint for ML predictions
- `/api/status` - GET endpoint to check server status

## Models

### MLP (sklearn)
- Architecture: 2 hidden layers (16, 8 neurons)
- Framework: scikit-learn
- Saved as: `mlp_model.joblib`

### PyTorch Neural Network
- Architecture: 2 hidden layers, 16 neurons each
- Framework: PyTorch
- Saved as: `pytorch_nn_model.pth`

### Random Forest
- Parameters: 100 trees, max depth 20
- Framework: scikit-learn
- Saved as: `random_forest_model.joblib`

## Data Format

All models expect the same input features:
- InternalResistance
- StartingInternalResistance
- Voltage
- Current
- Temp
- AccelX
- AccelY
- AccelCombined
- Speed
- Distance

Target: Level (1-10 performance classification)

## Workflow

1. **Setup Database:**
   ```bash
   python create_database.py
   ```

2. **Train Models:**
   ```bash
   python save_model.py
   ```

3. **Evaluate Models:**
   ```bash
   python evaluate_models.py
   ```

4. **Start Data Receiver:**
   ```bash
   cd ../data
   python data_receiver.py
   ```

5. **Use Predictions:**
   - ESP32 sends sensor data to `/api/data`
   - ESP32 requests predictions from `/api/predict`
   - Server returns predicted performance level (1-10)

## File Descriptions

- `create_database.py` - Creates SQLite schema and imports CSV data
- `preprocessing.py` - Processes raw sensor data into ML-ready format
- `save_model.py` - Trains and saves all three ML models
- `train_pytorch_nn.py` - PyTorch neural network training
- `train_random_forest.py` - Random Forest training
- `train_mlp.py` - MLP (sklearn) training
- `evaluate_models.py` - Comprehensive model evaluation and comparison
- `data_receiver.py` - Flask server for receiving data and providing predictions
