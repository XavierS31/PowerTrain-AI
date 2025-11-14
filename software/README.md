# PowerTrain AI - Battery Health Prediction System

Machine learning system for predicting battery health from ESP32 sensor data.

## Project Structure

```
software/
├── data/
│   ├── raw/            # Unprocessed ESP32 logs
│   ├── processed/      # Cleaned data
│   └── features/       # ML-ready datasets (X, y)
│
├── models/
│   ├── saved/          # Trained models
│   └── results/        # Metrics, plots
│
├── notebooks/
│   ├── 01_explore.ipynb    # Data exploration
│   ├── 02_features.ipynb   # Feature engineering
│   ├── 03_train.ipynb      # Model training
│   └── 04_eval.ipynb       # Model evaluation
│
├── src/
│   ├── ingestion.py        # Serial reading, log syncing
│   ├── preprocess.py       # Cleaning + normalization
│   ├── features.py         # Rint, motion, engineered features
│   ├── model.py            # PyTorch model definition
│   ├── train.py            # Training loop
│   ├── evaluate.py         # Evaluation metrics + plots
│   ├── infer.py            # Real-time battery predictions
│   └── utils.py            # Filters, helpers, constants
│
├── requirements.txt
└── README.md
```

## Installation

1. Install dependencies:
```bash
pip install -r requirements.txt
```

2. Set up Jupyter kernel:
```bash
python -m ipykernel install --user --name=powertrain-ai
```

## Usage

### Data Ingestion
```python
from src.ingestion import read_serial_data

# Read data from ESP32
for line in read_serial_data('COM3'):
    print(line)
```

### Training
```python
from src.train import train_model

# Train model
model, history = train_model(model, train_loader, val_loader, epochs=100, device='cuda')
```

### Inference
```python
from src.infer import predict_battery_health

# Make predictions
predictions = predict_battery_health(model, features)
```

## Development

Follow the notebook sequence:
1. `01_explore.ipynb` - Explore and understand the data
2. `02_features.ipynb` - Engineer features
3. `03_train.ipynb` - Train models
4. `04_eval.ipynb` - Evaluate and compare models

