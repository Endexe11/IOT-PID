<template>
  <div>
    <canvas ref="lineChartCanvas"></canvas>
    <div class="temperature-info">
      <p>Temperatura Actual: {{ latestActualTemperature }} °C</p>
      <p>Temperatura de Referencia: {{ latestReferenceTemperature }} °C</p>
    </div>
    <button @click="togglePause">{{ paused ? 'Reanudar' : 'Pausar' }}</button>
    <button @click="saveChart" :disabled="!paused">Guardar Gráfica</button>
    <button @click="deleteExperimentData" :disabled="!paused">Eliminar Datos del Experimento</button>
  </div>
</template>

<script>
import { ref as vueRef, onMounted, watch, onBeforeUnmount } from 'vue';
import { Chart } from 'chart.js/auto';
import 'chartjs-adapter-date-fns';
import { auth, database } from '../firebaseConfig';
import { ref as firebaseRef, onValue, off, remove, set } from 'firebase/database';
import { getStorage, ref as storageRef, uploadString, getDownloadURL } from 'firebase/storage';

export default {
  name: 'LineChart',
  props: {
    experimentData: {
      type: Object,
      required: true
    },
    experimentId: {
      type: Number,
      required: true
    }
  },
  setup(props) {
    const lineChartCanvas = vueRef(null);
    const localExperimentData = vueRef({ ...props.experimentData }); 
    let chart = null;
    let intervalId = null;
    const paused = vueRef(false);
    const latestActualTemperature = vueRef(null);
    const latestReferenceTemperature = vueRef(null);

    const updateChart = () => {
      if (lineChartCanvas.value) {
        const ctx = lineChartCanvas.value.getContext('2d');

        const dataEntries = Object.entries(localExperimentData.value); 
        const limitedDataEntries = dataEntries.slice(-500);

        const labels = limitedDataEntries.map(([timestamp]) => new Date(parseInt(timestamp)*1000));
        const actualTemperatures = limitedDataEntries.map(([, data]) => data.Temperatura_Actual);
        const referenceTemperatures = limitedDataEntries.map(([, data]) => data.Temperatura_Referencia);

        if (actualTemperatures.length > 0) {
          latestActualTemperature.value = actualTemperatures[actualTemperatures.length - 1];
        }

        if (referenceTemperatures.length > 0) {
          latestReferenceTemperature.value = referenceTemperatures[referenceTemperatures.length - 1];
        }

        if (chart) {
          chart.destroy();
        }

        chart = new Chart(ctx, {
          type: 'line',
          data: {
            labels: labels,
            datasets: [
              {
                label: 'Temperatura Actual',
                data: actualTemperatures,
                borderColor: 'rgba(75, 192, 192, 1)',
                backgroundColor: 'rgba(75, 192, 192, 0.2)',
                fill: false,
              },
              {
                label: 'Temperatura Referencia',
                data: referenceTemperatures,
                borderColor: 'rgba(255, 99, 132, 1)',
                backgroundColor: 'rgba(255, 99, 132, 0.2)',
                fill: false,
              }
            ]
          },
          options: {
            scales: {
              x: {
                type: 'time',
                time: {
                  unit: 'minute'
                },
                title: {
                  display: true,
                  text: 'Tiempo'
                }
              },
              y: {
                title: {
                  display: true,
                  text: 'Temperatura (°C)'
                }
              }
            }
          }
        });
      }
    };

    const startDataListening = () => {
      const user = auth.currentUser;
      if (user) {
        const experimentRef = firebaseRef(database, `UsersData/${user.uid}/Muestras_Temperatura/Experimento_${props.experimentId}`);
        onValue(experimentRef, snapshot => {
          const data = snapshot.val();
          if (data) {
            localExperimentData.value = data; 
          }
        });
      }
    };

    const stopDataListening = () => {
      const user = auth.currentUser;
      if (user) {
        const experimentRef = firebaseRef(database, `UsersData/${user.uid}/Muestras_Temperatura/Experimento_${props.experimentId}`);
        off(experimentRef);
      }
    };

    const saveChart = async () => {
      if (lineChartCanvas.value) {
        const user = auth.currentUser;
        if (user) {
          const storage = getStorage();
          const canvas = lineChartCanvas.value;
          const dataUrl = canvas.toDataURL('image/png');

          const storagePath = `Resultados_Experimentos/${user.uid}/Experimento_${props.experimentId}.png`;
          const storageReference = storageRef(storage, storagePath);
          await uploadString(storageReference, dataUrl, 'data_url');

          const downloadURL = await getDownloadURL(storageReference);

          const resultsRef = firebaseRef(database, `UsersData/${user.uid}/Resultados_Experimentos/Experimento_${props.experimentId}`);
          await set(resultsRef, {
            imageUrl: downloadURL,
            timestamp: new Date().toISOString()
          });

          alert('Gráfica guardada exitosamente.');
        }
      }
    };

    const deleteExperimentData = async () => {
      try {
        const user = auth.currentUser;
        if (user) {
          const experimentRef = firebaseRef(database, `UsersData/${user.uid}/Muestras_Temperatura/Experimento_${props.experimentId}`);
          await remove(experimentRef);
          localExperimentData.value = {}; // Limpiar la copia local
          updateChart();
          alert('Datos del experimento eliminados exitosamente.');
        }
      } catch (error) {
        console.error('Error al eliminar los datos del experimento:', error);
        alert('Ocurrió un error al eliminar los datos del experimento.');
      }
    };

    onMounted(() => {
      updateChart();
      startDataListening();
      intervalId = setInterval(updateChart, 5000);
    });

    watch(() => props.experimentData, (newVal) => {
      localExperimentData.value = { ...newVal }; // Sincronizar la copia local con los nuevos datos de la prop
      updateChart();
    });

    onBeforeUnmount(() => {
      stopDataListening();
      clearInterval(intervalId);
    });

    const togglePause = () => {
      paused.value = !paused.value;
      if (paused.value) {
        clearInterval(intervalId);
        stopDataListening();
      } else {
        updateChart();
        startDataListening();
        intervalId = setInterval(updateChart, 5000);
      }
    };

    return {
      lineChartCanvas,
      paused,
      latestActualTemperature,
      latestReferenceTemperature,
      togglePause,
      saveChart,
      deleteExperimentData
    };
  }
};
</script>

<style scoped>
canvas {
  max-width: 100%;
}
.temperature-info {
  margin: 10px 0;
}
button {
  padding: 10px;
  margin: 5px;
  background-color: #007bff;
  color: #fff;
  border: none;
  border-radius: 5px;
  cursor: pointer;
}
button:disabled {
  background-color: #d3d3d3;
  cursor: not-allowed;
}
</style>






