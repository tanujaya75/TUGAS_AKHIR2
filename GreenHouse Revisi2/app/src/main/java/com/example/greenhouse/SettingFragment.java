package com.example.greenhouse;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;

import androidx.core.view.accessibility.AccessibilityViewCommand;
import androidx.fragment.app.Fragment;

import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;

public class SettingFragment extends Fragment {
    EditText BatasAtasSuhuKipas, BatasBawahSuhuKipas, BatasAtasHumidityMisting, BatasBawahHumidityMisting, BatasAtasSuhuMisting, BatasBawahSuhuMisting, BatasBawahNutrisi, BatasAtasNutrisi, BatasBawahLux, BatasAtasLux;
    Handler handler = new Handler();

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_setting, container, false);
        BatasBawahSuhuKipas = view.findViewById(R.id.BatasBawahSuhu);
        BatasAtasSuhuKipas = view.findViewById(R.id.BatasAtasSuhu);
        BatasBawahHumidityMisting = view.findViewById(R.id.BatasBawahHumidity);
        BatasAtasHumidityMisting = view.findViewById(R.id.BatasAtasHumidity);
        BatasBawahSuhuMisting = view.findViewById(R.id.BatasBawahSuhuMisting);
        BatasAtasSuhuMisting = view.findViewById(R.id.BatasAtasSuhuMisting);
        BatasBawahNutrisi = view.findViewById(R.id.BatasBawahNustrisi);
        BatasAtasNutrisi = view.findViewById(R.id.BatasAtasNutrisi);
        BatasBawahLux = view.findViewById(R.id.BatasBawahLux);
        BatasAtasLux = view.findViewById(R.id.BatasAtasLux);

        BatasBawahSuhuKipas.setText(restoreSetting("BatasBawahSuhu"));
        BatasAtasSuhuKipas.setText(restoreSetting("BatasAtasSuhu"));
        BatasBawahHumidityMisting.setText(restoreSetting("BatasBawahHumidity"));
        BatasAtasHumidityMisting.setText(restoreSetting("BatasAtasHumidity"));
        BatasBawahSuhuMisting.setText(restoreSetting("BatasBawahSuhuMisting"));
        BatasAtasSuhuMisting.setText(restoreSetting("BatasAtasSuhuMisting"));
        BatasBawahNutrisi.setText(restoreSetting("BatasBawahNutrisi"));
        BatasAtasNutrisi.setText(restoreSetting("BatasAtasNutrisi"));
        BatasBawahLux.setText(restoreSetting("BatasBawahLux"));
        BatasAtasLux.setText(restoreSetting("BatasAtasLux"));

        Runnable updaterunnable = new Runnable()
        {
            @Override
            public void run()
            {

                String BatasBawahSuhu_Kipas= BatasBawahSuhuKipas.getText().toString();
                String BatasAtasSuhu_Kipas= BatasAtasSuhuKipas.getText().toString();
                String BatasBawahHumidity_Misting = BatasBawahHumidityMisting.getText().toString();
                String BatasAtasHumidity_Misting= BatasAtasHumidityMisting.getText().toString();
                String BatasBawahSuhu_Misting= BatasBawahSuhuMisting.getText().toString();
                String BatasAtasSuhu_Misting= BatasAtasSuhuMisting.getText().toString();
                String BatasBawah_Nutrisi= BatasBawahNutrisi.getText().toString();
                String BatasAtas_Nutrisi= BatasAtasNutrisi.getText().toString();
                String BatasBawah_Lux = BatasBawahLux.getText().toString();
                String BatasAtas_Lux = BatasAtasLux.getText().toString();

                Log.d("REST", "BatasBawahSuhu: " + BatasBawahSuhu_Kipas);
                Log.d("REST", "BatasAtasSuhu: " + BatasAtasSuhu_Kipas);
                Log.d("REST", "BatasBawahHumidity: " + BatasBawahHumidity_Misting);
                Log.d("REST", "BatasAtasHumidity: " + BatasAtasHumidity_Misting);
                Log.d("REST", "BatasBawahSuhuMisting: " + BatasBawahSuhu_Misting);
                Log.d("REST", "BatasAtasSuhuMisting: " + BatasAtasSuhu_Misting);
                Log.d("REST", "BatasBawahNutrisi: " + BatasBawah_Nutrisi);
                Log.d("REST", "BatasAtasNutrisi: " + BatasAtas_Nutrisi);
                Log.d("REST", "BatasBawahLux: " + BatasBawah_Lux);
                Log.d("REST", "BatasAtasLux: " + BatasAtas_Lux);

                saveSetting("BatasBawahSuhu", BatasBawahSuhu_Kipas);
                saveSetting("BatasAtasSuhu", BatasAtasSuhu_Kipas);
                saveSetting("BatasBawahHumidity", BatasBawahHumidity_Misting);
                saveSetting("BatasAtasHumidity", BatasAtasHumidity_Misting);
                saveSetting("BatasBawahSuhuMisting", BatasBawahSuhu_Misting);
                saveSetting("BatasAtasSuhuMisting", BatasAtasSuhu_Misting);
                saveSetting("BatasBawahNutrisi", BatasBawah_Nutrisi);
                saveSetting("BatasAtasNutrisi", BatasAtas_Nutrisi);
                saveSetting("BatasBawahLux", BatasBawah_Lux);
                saveSetting("BatasAtasLux", BatasAtas_Lux);
                handler.postDelayed(this, 500);
            }

        };
        handler.post(updaterunnable);




        return view;
    }

//    public void onPause()
//    {
//        super.onPause();
////        handler.removeCallbacksAndMessages(null);
//    }
    public void onStop() {
        super.onStop();
        Intent serviceIntent = new Intent(getActivity(), ControlFragment.class);
        getActivity().startService(serviceIntent);
    }
    private void saveSetting(String SettingName, String SettingConf) {
        // Save the state of the switch to SharedPreferences
        SharedPreferences preferences = requireActivity().getPreferences(Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = preferences.edit();
        editor.putString(SettingName, SettingConf);
        editor.apply();
    }
    private String restoreSetting(String SettingName)
    {
        SharedPreferences preferences = requireActivity().getPreferences(Context.MODE_PRIVATE);
        return preferences.getString(SettingName, "default_value");
    }
    public void onDestroyView()
    {
        super.onDestroyView();
        handler.removeCallbacksAndMessages(null);
    }
}