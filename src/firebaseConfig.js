// src/firebaseConfig.js
import { initializeApp } from 'firebase/app';
import { getAuth } from 'firebase/auth';
import { getDatabase } from 'firebase/database';

const firebaseConfig = {
  apiKey: "AIzaSyChB8SAFKTAh7y4I6RdsW_RgdwDSJh1fEA",
  authDomain: "led1-a76d6.firebaseapp.com",
  databaseURL: "https://led1-a76d6-default-rtdb.firebaseio.com",
  projectId: "led1-a76d6",
  storageBucket: "led1-a76d6.appspot.com",
  messagingSenderId: "371682626728",
  appId: "1:371682626728:web:5f3cfcaf7d0f53c1d0869d",
  measurementId: "G-E6TGBG3B2X"
};

const app = initializeApp(firebaseConfig);
const auth = getAuth(app);
const database = getDatabase(app);

export { auth, database };
