@echo off
title PowerTrain AI - Run MLP Model
color 0D
echo.
echo PowerTrain AI - MLP Model Training
echo.
echo This will train the MLP model and generate
echo a confusion matrix visualization
echo.
echo.

cd /d "%~dp0Software\src"

echo.
echo Checking Python environment...
python --version
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: Python not found!
    echo Please install Python and try again
    echo.
    pause
    exit /b 1
)

echo.
echo Installing required packages (if needed)...
pip install pandas numpy scikit-learn matplotlib seaborn --quiet

echo Running MLP Model Training
echo.

python train_mlp.py

if %ERRORLEVEL% EQU 0 (
    echo.
    echo MODEL TRAINING COMPLETE!
    echo.
    echo Results saved to: Software\models\results\confusion_matrix.png
    echo.
) else (
    echo.
    echo MODEL TRAINING FAILED!
    echo.
    echo Please check the error messages above
    echo Make sure the dataset file exists:
    echo   Software\data\raw_data\driving_behavior_dataset.csv
    echo.
)

pause


