import pyarrow as pa
import pyarrow.ipc as ipc
import numpy as np
from sklearn.ensemble import IsolationForest
import matplotlib.pyplot as plt

with pa.memory_map("../data/hep_data.arrow", "r") as source:
    reader = ipc.open_file(source)
    table = reader.read_all()

df = table.to_pandas()

X = df[["th_x", "th_y", "t"]]

model = IsolationForest(n_estimators=100, contamination=0.01, random_state=42)
df["Anomaly"] = model.fit_predict(X)

new_table = pa.Table.from_pandas(df)

with pa.OSFile("../results/anomalies.arrow", "wb") as sink:
    writer = ipc.new_file(sink, new_table.schema)
    writer.write_table(new_table)
    writer.close()

print(f"✅ Anomalies detected: {df['Anomaly'].value_counts().get(-1, 0)}")
