import time
import pandas as pd
from sklearn.datasets import fetch_openml
from sklearn.model_selection import train_test_split
from sklearn.neighbors import KNeighborsClassifier
from sklearn.metrics import accuracy_score, classification_report

fashion_mnist = fetch_openml(
name='Fashion-MNIST',
version=1,
as_frame=False,
parser='auto'
)

X = fashion_mnist.data
y = fashion_mnist.target

print(f"Original Dataset Shape: {X.shape}")

X_subset, _, y_subset, _ = train_test_split(
X,
y,
train_size=12000,
stratify=y,
random_state=42
)

X_scaled = X_subset / 255.0

X_train, X_test, y_train, y_test = train_test_split(
X_scaled,
y_subset,
test_size=2000,
random_state=42,
stratify=y_subset
)

print(f"Training Set Shape : {X_train.shape}")
print(f"Testing Set Shape : {X_test.shape}\n")

k_values = [1, 3, 5, 7, 15]
results = []

print("==========================================")
print(f"{'K-Value':<10} | {'Accuracy':<10} | {'Prediction Time':<15}")
print("==========================================")

for k in k_values:
knn = KNeighborsClassifier(n_neighbors=k)
knn.fit(X_train, y_train)

start_time = time.time()
y_pred = knn.predict(X_test)
end_time = time.time()

acc = accuracy_score(y_test, y_pred)
time_taken = end_time - start_time

results.append({
    "K": k,
    "Accuracy": acc,
    "Time (s)": time_taken
})

print(
    f"K = {k:<5} | "
    f"{acc:<10.4f} | "
    f"{time_taken:<15.4f} seconds"
)
results_df = pd.DataFrame(results)

best_k = 5

print(f"\nClassification Report for Optimal K ({best_k}):")

best_knn = KNeighborsClassifier(n_neighbors=best_k)
best_knn.fit(X_train, y_train)

print(
classification_report(
y_test,
best_knn.predict(X_test)
)
)


