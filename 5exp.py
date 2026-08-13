import numpy as np
from sklearn.datasets import fetch_20newsgroups
from sklearn.feature_extraction.text import CountVectorizer

data = fetch_20newsgroups(
subset='train',
remove=('headers', 'footers', 'quotes')
)

print("Number of documents:", len(data.data))

vectorizer = CountVectorizer(
stop_words='english',
min_df=5
)

X = vectorizer.fit_transform(data.data)

words = vectorizer.get_feature_names_out()

print("Vocabulary size:", len(words))

word_counts = np.asarray(X.sum(axis=0)).flatten()

total_words = word_counts.sum()

mle = word_counts / total_words

def map_estimate(counts, alpha):

V = len(counts)
N = counts.sum()

map_prob = (counts + alpha - 1) / (N + V * (alpha - 1))

return map_prob
Different priors
map_01 = map_estimate(word_counts, 0.1)
map_1 = map_estimate(word_counts, 1)
map_10 = map_estimate(word_counts, 10)

def show_top_words(probabilities, title, n=10):

indices = np.argsort(probabilities)[-n:][::-1]

print("\n", title)

for i in indices:
    print(words[i], ":", probabilities[i])
show_top_words(mle, "MLE")

show_top_words(map_01, "MAP - alpha = 0.1")

show_top_words(map_1, "MAP - alpha = 1")

show_top_words(map_10, "MAP - alpha = 10")


print("\nComparison:")

for word in ["computer", "science", "government", "people"]:

if word in vectorizer.vocabulary_:

    i = vectorizer.vocabulary_[word]

    print("\nWord:", word)
    print("MLE      :", mle[i])
    print("MAP 0.1  :", map_01[i])
    print("MAP 1    :", map_1[i])
    print("MAP 10   :", map_10[i])
