import pandas as pd
from sklearn.datasets import fetch_20newsgroups
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.naive_bayes import MultinomialNB, BernoulliNB
from sklearn.metrics import accuracy_score, f1_score, classification_report

categories = ['alt.atheism', 'comp.graphics', 'sci.space', 'talk.religion.misc']

train_data = fetch_20newsgroups(
subset='train',
categories=categories,
remove=('headers', 'footers', 'quotes')
)

test_data = fetch_20newsgroups(
subset='test',
categories=categories,
remove=('headers', 'footers', 'quotes')
)

y_train = train_data.target
y_test = test_data.target

vectorizer = CountVectorizer(
stop_words='english',
max_features=5000
)

X_train = vectorizer.fit_transform(train_data.data)
X_test = vectorizer.transform(test_data.data)

def evaluate_model(model, name):
y_pred = model.predict(X_test)

print("==============================")
print(name)
print("==============================")

acc = accuracy_score(y_test, y_pred)
f1 = f1_score(y_test, y_pred, average='weighted')

print(f"Accuracy : {acc:.4f}")
print(f"F1 Score : {f1:.4f}")
print("\nClassification Report:")
print(classification_report(
    y_test,
    y_pred,
    target_names=train_data.target_names
))

return {
    "Accuracy": acc,
    "F1 Score": f1
}
mnb_model = MultinomialNB()
mnb_model.fit(X_train, y_train)

metrics_mnb = evaluate_model(
mnb_model,
"Multinomial Naïve Bayes"
)

bnb_model = BernoulliNB(binarize=0.0)
bnb_model.fit(X_train, y_train)

metrics_bnb = evaluate_model(
bnb_model,
"Bernoulli Naïve Bayes"
)

comparison_df = pd.DataFrame({
"Metric": ["Accuracy", "F1 Score (Weighted)"],
"Multinomial NB": [
metrics_mnb['Accuracy'],
metrics_mnb['F1 Score']
],
"Bernoulli NB": [
metrics_bnb['Accuracy'],
metrics_bnb['F1 Score']
]
})

print("\n==========================================")
print("Performance Comparison")
print("==========================================")
print(comparison_df.to_string(index=False))
