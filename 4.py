import numpy as np
import pandas as pd
from sklearn.datasets import load_breast_cancer
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.metrics import accuracy_score, precision_score, recall_score, f1_score, roc_auc_score
from scipy.optimize import minimize

# --------------------------------------------------
# 1. Load and preprocess the dataset
# --------------------------------------------------

data = load_breast_cancer()

X = data.data
y = data.target

# Standardize features
scaler = StandardScaler()
X = scaler.fit_transform(X)

# Add intercept column
X = np.c_[np.ones(X.shape[0]), X]

# Train-test split
X_train, X_test, y_train, y_test = train_test_split(
    X, y,
    test_size=0.2,
    random_state=42,
    stratify=y
)

# --------------------------------------------------
# 2. Logistic regression functions
# --------------------------------------------------

def sigmoid(z):
    z = np.clip(z, -500, 500)
    return 1 / (1 + np.exp(-z))


def negative_log_likelihood(beta, X, y):
    """
    Negative log-likelihood for MLE.
    """
    z = X @ beta

    # Numerically stable negative log-likelihood
    return np.sum(np.logaddexp(0, z) - y * z)


# --------------------------------------------------
# 3. MLE estimation
# --------------------------------------------------

beta_initial = np.zeros(X_train.shape[1])

mle_result = minimize(
    negative_log_likelihood,
    beta_initial,
    args=(X_train, y_train),
    method="BFGS"
)

beta_mle = mle_result.x

print("MLE optimization successful:", mle_result.success)


# --------------------------------------------------
# 4. MAP estimation with L2 prior
# --------------------------------------------------

def negative_log_posterior_l2(beta, X, y, lam):
    """
    Negative log-posterior with Gaussian prior.

    Gaussian prior:
        p(beta) ∝ exp(-lambda * ||beta||² / 2)
    """

    likelihood = negative_log_likelihood(beta, X, y)

    # Do not regularize the intercept
    regularization = (lam / 2) * np.sum(beta[1:] ** 2)

    return likelihood + regularization


# --------------------------------------------------
# 5. MAP estimation with L1 prior
# --------------------------------------------------

def negative_log_posterior_l1(beta, X, y, lam):
    """
    Negative log-posterior with Laplace prior.

    Laplace prior:
        p(beta) ∝ exp(-lambda * ||beta||₁)
    """

    likelihood = negative_log_likelihood(beta, X, y)

    # Do not regularize the intercept
    regularization = lam * np.sum(np.abs(beta[1:]))

    return likelihood + regularization


# --------------------------------------------------
# 6. Estimate MAP parameters for different lambda
# --------------------------------------------------

lambdas = [0.01, 0.1, 1, 10]

l2_models = {}
l1_models = {}

for lam in lambdas:

    # L2 MAP
    result_l2 = minimize(
        negative_log_posterior_l2,
        beta_initial,
        args=(X_train, y_train, lam),
        method="BFGS"
    )

    l2_models[lam] = result_l2.x

    # L1 MAP
    result_l1 = minimize(
        negative_log_posterior_l1,
        beta_initial,
        args=(X_train, y_train, lam),
        method="BFGS"
    )

    l1_models[lam] = result_l1.x


# --------------------------------------------------
# 7. Prediction function
# --------------------------------------------------

def predict(X, beta):
    probability = sigmoid(X @ beta)

    prediction = (probability >= 0.5).astype(int)

    return prediction, probability


# --------------------------------------------------
# 8. Evaluate models
# --------------------------------------------------

def evaluate_model(name, beta):

    y_pred, y_prob = predict(X_test, beta)

    accuracy = accuracy_score(y_test, y_pred)
    precision = precision_score(y_test, y_pred)
    recall = recall_score(y_test, y_pred)
    f1 = f1_score(y_test, y_pred)
    auc = roc_auc_score(y_test, y_prob)

    return {
        "Model": name,
        "Accuracy": accuracy,
        "Precision": precision,
        "Recall": recall,
        "F1 Score": f1,
        "ROC-AUC": auc
    }


results = []

# MLE
results.append(
    evaluate_model("MLE", beta_mle)
)

# L2 MAP
for lam, beta in l2_models.items():
    results.append(
        evaluate_model(f"MAP-L2 (λ={lam})", beta)
    )

# L1 MAP
for lam, beta in l1_models.items():
    results.append(
        evaluate_model(f"MAP-L1 (λ={lam})", beta)
    )


# --------------------------------------------------
# 9. Display performance comparison
# --------------------------------------------------

results_df = pd.DataFrame(results)

print("\nModel Performance:")
print(results_df.to_string(index=False))


# --------------------------------------------------
# 10. Compare parameter estimates
# --------------------------------------------------

parameter_names = ["Intercept"] + list(data.feature_names)

parameter_table = pd.DataFrame({
    "Feature": parameter_names,
    "MLE": beta_mle
})

# Add L2 parameters
for lam, beta in l2_models.items():
    parameter_table[f"L2_lambda_{lam}"] = beta

# Add L1 parameters
for lam, beta in l1_models.items():
    parameter_table[f"L1_lambda_{lam}"] = beta

print("\nParameter Estimates:")
print(parameter_table.to_string(index=False))


# --------------------------------------------------
# 11. Examine effect of regularization
# --------------------------------------------------

print("\nCoefficient L2 Norms:")

print("MLE:",
      np.linalg.norm(beta_mle[1:], ord=2))

for lam, beta in l2_models.items():
    print(
        f"L2 λ={lam}:",
        np.linalg.norm(beta[1:], ord=2)
    )


print("\nCoefficient L1 Norms:")

print("MLE:",
      np.linalg.norm(beta_mle[1:], ord=1))

for lam, beta in l1_models.items():
    print(
        f"L1 λ={lam}:",
        np.linalg.norm(beta[1:], ord=1)
    )


# --------------------------------------------------
# 12. Count coefficients close to zero
# --------------------------------------------------

print("\nNumber of coefficients close to zero:")

for lam, beta in l1_models.items():

    zero_count = np.sum(
        np.abs(beta[1:]) < 1e-3
    )

    print(
        f"L1 λ={lam}: {zero_count}"
    )
