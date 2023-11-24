package com.example.greenhouse;

import android.content.Context;
import android.content.res.Configuration;
import android.graphics.Color;
import android.os.Bundle;

import androidx.fragment.app.Fragment;

import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.components.AxisBase;
import com.github.mikephil.charting.components.XAxis;
import com.github.mikephil.charting.components.YAxis;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;
import com.github.mikephil.charting.formatter.ValueFormatter;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.sql.Time;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Random;
import java.util.concurrent.Executor;
import java.util.concurrent.Executors;

public class HomeFragment extends Fragment
{
    TextView Suhu_TV,Kelembapan_TV, Nutrisi_TV, Lux_TV, PB_TV, TIME_TV;
    Random random = new Random();
    Context context;

    Handler handler = new Handler();
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState)
    {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_home, container, false);
        Suhu_TV = view.findViewById(R.id.Suhu_TextView);
        Kelembapan_TV = view.findViewById(R.id.Kelembapan_TextView);
        Nutrisi_TV = view.findViewById(R.id.Nutrisi_TextView);
        Lux_TV = view.findViewById(R.id.Lux_TextView);
        PB_TV = view.findViewById(R.id.PB_TextView);
        TIME_TV = view.findViewById(R.id.Time_TV);
        Executor executor1 = Executors.newSingleThreadExecutor();
        context = requireContext().getApplicationContext();
        final String[] s = {null};
        final String[] s2 = {null};


        Runnable updateRunnable = new Runnable() 
        {

            int Suhu;
            int Kelembapan;
            int Nutrisi;
            int Cahaya;
            int P_Barometrik;
            String Type;
            @Override
            public void run() 
            {
                // Simulate getting a new temperature value

                executor1.execute(() -> {
                    HTTPHANDLER httpHandler2 = new HTTPHANDLER();
                    String u = Link.url3;
                    String u2 = Link.url2;
                    s[0] = httpHandler2.getAccess(u);
                    s2[0] = httpHandler2.getAccess(u2);
                });

                System.out.print("Return From Antares:"+s[0]);
                Log.d("REST", "Return From Antares:" + s[0]);
                Log.d("REST", "Return From Antares2:" + s2[0]);

                if ( s[0] != null)
                {
                    try {
                        String JsonString = s[0];
                        JSONObject JsonObject = new JSONObject(JsonString);
                        JSONObject CinObject = JsonObject.getJSONObject("m2m:cin");
                        String ConString = CinObject.getString("con");
                        JSONObject SensorData_Antares = new JSONObject(ConString);

                        Suhu = SensorData_Antares.getInt("Suhu");
                        Kelembapan = SensorData_Antares.getInt("Kelembapan");
                        P_Barometrik = SensorData_Antares.getInt("Tekanan");
                        Cahaya = SensorData_Antares.getInt("Lux");
                        Nutrisi = SensorData_Antares.getInt("EC");
                        Type = SensorData_Antares.getString("Type");

                        Log.d("D", "Suhu: " + Suhu);
                        Log.d("D", "Kelembapan: " + Kelembapan);
                        Log.d("D", "Tekanan: " + P_Barometrik);
                        Log.d("D", "Lux: " + Cahaya);
                        Log.d("D", "EC: " + Nutrisi);
                        Log.d("D", "Type: " + Type);
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                }

                Suhu_TV.setText(Suhu + "°C");
                Kelembapan_TV.setText(Kelembapan + "%");
                Nutrisi_TV.setText(Nutrisi + " PPM");
                Lux_TV.setText(Cahaya+"L");
                PB_TV.setText(P_Barometrik+" hPa");


                if (s2[0] != null)
                {
                    String GrafikDT = s2[0];
                    try
                    {
                        JSONObject GrafikData = new JSONObject(GrafikDT);
                        JSONArray GrafikArray = GrafikData.getJSONArray("m2m:list");
                        int[] Sensor_Suhu = new int[GrafikArray.length()];
                        String[] Time = new String[GrafikArray.length()];
                        String [] Data_To_Parse = new String[GrafikArray.length()];
                        int SensorSuhu;
                        String Waktu;
                        int LastIndex = Math.min(20, GrafikArray.length());
                        System.out.println(LastIndex);

                        for (int i = 0; i <LastIndex ; i++)
                        {
//                            System.out.println(i);


                            JSONObject GrafikCinObject = GrafikArray.getJSONObject(i).getJSONObject("m2m:cin");
                            String GrafikConString = GrafikCinObject.getString("con");
                            String TimeCT = GrafikCinObject.getString("ct");

                            JSONObject NewGrafikObject = new JSONObject(GrafikConString);
//                            JSONObject NewTimeObject = new JSONObject(TimeCT);

                            System.out.println(NewGrafikObject);


                            SimpleDateFormat inputFormat = new SimpleDateFormat("yyyyMMdd'T'HHmmss", Locale.getDefault());
                            Date Date = inputFormat.parse(TimeCT);
                            SimpleDateFormat outputFormat = new SimpleDateFormat("dd-MM-yyyy HH:mm:ss", Locale.getDefault());
                            String FormattedDate = outputFormat.format(Date);

//                            TIME_TV.setText("Last Update: "+FormattedDate);

                            System.out.println(FormattedDate);

                            if(!GrafikConString.isEmpty())
                            {
                                SensorSuhu = NewGrafikObject.getInt("Suhu");
                                //System.out.println(SensorUltrasonicData);
                                Sensor_Suhu[i] = SensorSuhu;
                                Time[i] = FormattedDate;
                                TIME_TV.setText("Last Update: "+Time[0]);
                            }
                            Log.d("REST", "Sensor Suhu: " + Sensor_Suhu[i]);
                            Log.d("REST", "Waktu: " + Time[i]);
                        }

                        int StartIndex = Math.min(20, Sensor_Suhu .length);
                        LineChart Linechart = view.findViewById(R.id.Chart_Suhu);


                        List<Entry> EntriesSuhu = new ArrayList<>();
                        List<Entry> EntriesTime = new ArrayList<>();
                        System.out.println("Start Index: "+StartIndex);


                        for (int j = StartIndex - 1, l =0 ; j >= 0; j--, l++)
                        {
                            System.out.println("Index: "+j);

                            EntriesSuhu.add(new Entry(l, Sensor_Suhu[j]));

                            //System.out.println(SensorUltrasonic[j]);

                            //Thread.sleep(1000);
                        }

                        LineDataSet dataSet = new LineDataSet(EntriesSuhu, "Sensor Suhu");
                        dataSet.setColor(R.color.dg);
                        dataSet.setLineWidth(2f);
                        LineData lineData = new LineData(dataSet);

                        Linechart.setData(lineData);

                        // Customizing chart appearance
                        XAxis xAxis = Linechart.getXAxis();
                        xAxis.setValueFormatter(new ValueFormatter()
                        {
                            public String getFormattedValue(float value)
                            {
                                return String.valueOf((long) value);
                            }
                        });
                        xAxis.setAxisLineWidth(1f);
                        xAxis.setAxisLineColor(R.color.dg);
                        xAxis.setPosition(XAxis.XAxisPosition.BOTTOM);

                        YAxis yAxis = Linechart.getAxisLeft();
                        yAxis.setValueFormatter(new ValueFormatter()
                        {

                            public String getFormattedValue(float value, AxisBase axis)
                            {
                                return super.getFormattedValue((int) value, axis);
                            }
                        });
                        yAxis.setAxisLineWidth(1f);
                        yAxis.setAxisLineColor(R.color.dg);

                        Linechart.invalidate();


                    }
                    catch (JSONException ex)
                    {
                        ex.printStackTrace();
                    }
                    catch (ParseException ex2)
                    {
                        ex2.printStackTrace();
                    }
                }

                handler.postDelayed(this, 1000); // 10000 milliseconds = 10 seconds
            }
        };
        handler.post(updateRunnable);

        return view;
    }
    public void onPause()
    {
        super.onPause();
        handler.removeCallbacksAndMessages(null);
    }
    public void onDestroyView()
    {
        super.onDestroyView();
        handler.removeCallbacksAndMessages(null);
    }
}