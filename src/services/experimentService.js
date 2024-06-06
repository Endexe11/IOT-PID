import { ref, set, onValue } from "firebase/database";
import { database } from "@/firebaseConfig";

function generateExperimentNumber(uid) {
  return new Promise((resolve, reject) => {
    const experimentRef = ref(database, `UsersData/${uid}/Muestras_Temperatura`);
    onValue(experimentRef, (snapshot) => {
      const experiments = snapshot.val();
      const experimentNumber = experiments ? Object.keys(experiments).length + 1 : 1;
      resolve(experimentNumber);
    }, {
      onlyOnce: true,
      errorCallback: (error) => reject(error)
    });
  });
}

function setParametersToExperiment(params, uid, numberExp,) {
  return new Promise(async (resolve, reject) => {
    try {
      await set(ref(database, `UsersData/${uid}/Activar_Experimento`), numberExp);
      await set(ref(database, `UsersData/${uid}/Valores_Referencia/Experimento${numberExp}`), params);
      resolve();
    } catch (error) {
      reject(error);
    }
  });
}

function getExperimentData(uid, experimentId, callback) {
  const experimentRef = ref(database, `UsersData/${uid}/Muestras_Temperatura/Experimento_${experimentId}`);
  onValue(experimentRef, (snapshot) => {
    callback(snapshot.val());
  }, {
    errorCallback: (error) => console.error(error)
  });
}

function setExperimentStarted(uid, experimentId, started) {
  return set(ref(database, `UsersData/${uid}/Resetear_Experimento`), started);
}
export default {
  generateExperimentNumber,
  setParametersToExperiment,
  getExperimentData,
  setExperimentStarted,
};