package com.example.greenhouse;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;

import androidx.fragment.app.Fragment;

import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CompoundButton;
import android.widget.Switch;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.concurrent.Executor;
import java.util.concurrent.Executors;


public class ControlFragment extends Fragment {
    Switch Automated, PompaAir, Lampu, Kipas, PompaMisting, PompaNutrisi;
    Handler handler = new Handler();
    Executor executor = Executors.newSingleThreadExecutor();
    boolean AutoPrev=false;
    boolean PumpPrev=false;
    boolean LampPrev=false;
    boolean FanPrev=false;
    boolean MistPrev=false;
    boolean NutritionPrev = false;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        Intent serviceIntent = new Intent(getActivity(), ControlFragment.class);
        getActivity().stopService(serviceIntent);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState)
    {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_control, container, false);
        Automated = view.findViewById(R.id.SwitchAuto);
        PompaAir = view.findViewById(R.id.SwitchPompaAir);
        Lampu = view.findViewById(R.id.SwitchLampu);
        Kipas = view.findViewById(R.id.SwitchKipas);
        PompaMisting = view.findViewById(R.id.SwitchMisting);
        PompaNutrisi = view.findViewById(R.id.SwitchPompaNutrisi);

        Automated.setChecked(restoreSwitchState("Auto"));
        PompaAir.setChecked(restoreSwitchState("PompaAir"));
        Lampu.setChecked(restoreSwitchState("Lampu"));
        Kipas.setChecked(restoreSwitchState("Kipas"));
        PompaMisting.setChecked(restoreSwitchState("Misting"));
        PompaNutrisi.setChecked(restoreSwitchState("PompaNutrisi"));



        Runnable updaterunnable = new Runnable()
        {
            @Override
            public void run()
            {

                boolean Auto = AutoSWT();
                boolean Pump = PompaAirSWT();
                boolean Lamp = LampuSWT();
                boolean Fan = KipasSWT();
                boolean Mist = PompaMistingSWT();
                boolean Nutrition = PompaNutrisiSWT();

                String BatasBawahSuhu_Kipas = GetSetting("BatasBawahSuhu");
                String BatasAtasSuhu_Kipas = GetSetting("BatasAtasSuhu");
                String BatasBawahHumidity_Misting = GetSetting("BatasBawahHumidity");
                String BatasAtasHumidity_Misting = GetSetting("BatasAtasHumidity");
                String BatasBawahSuhu_Misting = GetSetting("BatasBawahSuhuMisting");
                String BatasAtasSuhu_Misting = GetSetting("BatasAtasSuhuMisting");
                String BatasBawah_Nutrisi = GetSetting("BatasBawahNutrisi");
                String BatasAtas_Nutrisi = GetSetting("BatasAtasNutrisi");
                String BatasBawah_Lux = GetSetting("BatasBawahLux");
                String BatasAtas_Lux = GetSetting("BatasAtasLux");

                Log.d("REST", "AutoSWT: " + Auto);
                Log.d("REST", "PompaSWT: " + Pump);
                Log.d("REST", "LampuSWT: " + Lamp);
                Log.d("REST", "KipasSWT: " + Fan);
                Log.d("REST", "MistingSWT: " + Mist);
                Log.d("REST", "PompaNutrisiSWT: " + Nutrition);
                ArrayList<String> mkey = new ArrayList<String>();
                ArrayList mval = new ArrayList();
                JSONObject JSDATA = new JSONObject();
                JSONObject PayloadData = new JSONObject();

                try {
                    if (Auto == true) {
                        JSDATA.put("Automated", "HIGH");
                    } else {
                        JSDATA.put("Automated", "LOW");
                    }

                    if (Pump == true) {
                        JSDATA.put("Pompa", "HIGH");
                    } else {
                        JSDATA.put("Pompa", "LOW");
                    }
                    if (Lamp == true) {
                        JSDATA.put("Lampu", "HIGH");
                    } else {
                        JSDATA.put("Lampu", "LOW");
                    }
                    if (Fan == true) {
                        JSDATA.put("Kipas", "HIGH");
                    } else {
                        JSDATA.put("Kipas", "LOW");
                    }
                    if (Mist == true) {
                        JSDATA.put("Misting", "HIGH");
                    } else {
                        JSDATA.put("Misting", "LOW");
                    }
                    if (Nutrition == true) {
                        JSDATA.put("PompaNutrisi", "HIGH");
                    } else {
                        JSDATA.put("PompaNutrisi", "LOW");
                    }
                    JSDATA.put("Type", "Down");
                    JSDATA.put("BatasBawahSuhu", BatasBawahSuhu_Kipas);
                    JSDATA.put("BatasAtasSuhu", BatasAtasSuhu_Kipas);
                    JSDATA.put("BatasBawahHumidity", BatasBawahHumidity_Misting);
                    JSDATA.put("BatasAtasHumidity", BatasAtasHumidity_Misting);
                    JSDATA.put("BatasBawahSuhuMisting", BatasBawahSuhu_Misting);
                    JSDATA.put("BatasAtasSuhuMisting", BatasAtasSuhu_Misting);
                    JSDATA.put("BatasBawahNutrisi", BatasBawah_Nutrisi);
                    JSDATA.put("BatasAtasNutrisi", BatasAtas_Nutrisi);
                    JSDATA.put("BatasBawahLux", BatasBawah_Lux );
                    JSDATA.put("BatasAtasLux", BatasAtas_Lux);

                    PayloadData.put("con", JSDATA.toString());
                    mkey.add("m2m:cin");
                    mval.add(PayloadData);
                    if (Auto != AutoPrev || Pump != PumpPrev || Lamp != LampPrev || Fan != FanPrev || Mist != MistPrev || Nutrition != NutritionPrev)
                    {
                    executor.execute(() -> {
                        HTTPHANDLER httpHandler = new HTTPHANDLER();
                        String u = Link.url;
                        String s = httpHandler.POSTACCESS(u, mkey, mval);
                        Log.d("REST", "Response: " + s);
                    });

                        AutoPrev = Auto;
                        PumpPrev = Pump;
                        LampPrev = Lamp;
                        FanPrev = Fan;
                        MistPrev = Mist;
                        NutritionPrev = Nutrition;
                    }
                }
                catch (JSONException e)
                {
                    e.printStackTrace();
                }

                handler.postDelayed(this, 500);
            }
        };

        handler.post(updaterunnable);

        return view;
    }


    public boolean AutoSWT()
    {
        Automated.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener()
        {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean switchStat1)
            {
                saveSwitchState("Auto", switchStat1);
            }
        });
        return Automated.isChecked();
    }
    public boolean PompaAirSWT()
    {
        PompaAir.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener()
        {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean switchStat2)
            {
                saveSwitchState("PompaAir", switchStat2);
            }
        });
        return PompaAir.isChecked();
    }
    public boolean LampuSWT()
    {
        Lampu.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener()
        {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean switchStat3)
            {
                saveSwitchState("Lampu", switchStat3);
            }
        });
        return Lampu.isChecked();
    }

    public boolean KipasSWT()
    {
        Kipas.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener()
        {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean switchStat4)
            {
                saveSwitchState("Kipas", switchStat4);
            }
        });
        return Kipas.isChecked();
    }
    public boolean PompaMistingSWT()
    {
        PompaMisting.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener()
        {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean switchStat5)
            {
                saveSwitchState("Misting", switchStat5);
            }
        });
        return PompaMisting.isChecked();
    }

    public boolean PompaNutrisiSWT()
    {
        PompaNutrisi.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean switchStat6)
            {
                saveSwitchState("PompaNutrisi", switchStat6);
            }
        });
        return PompaNutrisi.isChecked();
    }

    public void onPause()
    {
        super.onPause();
//        handler.removeCallbacksAndMessages(null);
    }
    public void onStop() {
        super.onStop();
        Intent serviceIntent = new Intent(getActivity(), ControlFragment.class);
        getActivity().startService(serviceIntent);
    }
    private void saveSwitchState(String SwitchName, boolean SwitchState) {
        // Save the state of the switch to SharedPreferences
        SharedPreferences preferences = requireActivity().getPreferences(Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = preferences.edit();
        editor.putBoolean(SwitchName, SwitchState);
        editor.apply();
    }
    private boolean restoreSwitchState(String SwitchName) {
        SharedPreferences preferences = requireActivity().getPreferences(Context.MODE_PRIVATE);
        // If the key is not found, the default value (false in this case) will be returned
        return preferences.getBoolean(SwitchName, false);

    }
    private String GetSetting(String SettingName)
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

