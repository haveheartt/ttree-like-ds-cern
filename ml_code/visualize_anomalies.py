import pyarrow as pa
import pyarrow.compute as pc
import pyarrow.ipc as ipc
import matplotlib.pyplot as plt

with pa.memory_map("../results/anomalies.arrow", "r") as source:
    reader = ipc.open_file(source)
    table = reader.read_all()

anomalies = table.filter(pc.equal(table["Anomaly"], -1))
df_anomalies = anomalies.to_pandas()

plt.scatter(df_anomalies["th_x"], df_anomalies["th_y"], c="red", marker="o")
plt.xlabel("Theta X")
plt.ylabel("Theta Y")
plt.title("Anomaly Detection in Proton Scattering")
plt.show()
