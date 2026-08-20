import numpy as np
import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import accuracy_score, precision_score, recall_score, f1_score

#1. Load Dataset
url = "https://raw.githubusercontent.com/jbrownlee/Datasets/master/pima-indians-diabetes.data.csv"
columns = [
"Pregnancies", "Glucose", "BloodPressure", "SkinThickness",
"Insulin", "BMI", "DiabetesPedigree", "Age", "Outcome"
]
df = pd.read_csv(url, names=columns)

#2. Clean Data (Replace invalid 0s with column medians)
zero_cols = ["Glucose", "BloodPressure", "SkinThickness", "Insulin", "BMI"]
df[zero_cols] = df[zero_cols].replace(0, np.nan)
df[zero_cols] = df[zero_cols].fillna(df[zero_cols].median())

#3. Split Features and Target
X = df.drop(columns=["Outcome"])
y = df["Outcome"]

X_train, X_test, y_train, y_test = train_test_split(
X, y, test_size=0.2, random_state=42, stratify=y
)

Helper function to compute metrics
def compute_metrics(y_true, y_pred):
return {
"Accuracy": accuracy_score(y_true, y_pred),
"Precision": precision_score(y_true, y_pred),
"Recall": recall_score(y_true, y_pred),
"F1-Score": f1_score(y_true, y_pred)
}

#4. Logistic Regression Without Feature Scaling
clf_unscaled = LogisticRegression(max_iter=1000, random_state=42)
clf_unscaled.fit(X_train, y_train)
y_pred_unscaled = clf_unscaled.predict(X_test)
metrics_unscaled = compute_metrics(y_test, y_pred_unscaled)

#5. Apply Feature Scaling
scaler = StandardScaler()
X_train_scaled = scaler.fit_transform(X_train)
X_test_scaled = scaler.transform(X_test)

#6. Logistic Regression With Feature Scaling
clf_scaled = LogisticRegression(max_iter=1000, random_state=42)
clf_scaled.fit(X_train_scaled, y_train)
y_pred_scaled = clf_scaled.predict(X_test_scaled)
metrics_scaled = compute_metrics(y_test, y_pred_scaled)

#7. Print Performance Comparison
results_df = pd.DataFrame([metrics_unscaled, metrics_scaled], index=["Without Scaling", "With Scaling"])
print("=== Model Performance Comparison ===")
print(results_df.round(4))


