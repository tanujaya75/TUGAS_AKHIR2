package com.example.greenhouse;

import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;
import android.app.Activity;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import com.google.android.material.bottomnavigation.BottomNavigationView;

public class Home extends AppCompatActivity {
    BottomNavigationView NavigationView;
    Intent I;
    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_home);
        getSupportActionBar().hide();
        NavigationView = findViewById(R.id.bottomNavigationView);

        getSupportFragmentManager().beginTransaction().replace(R.id.FrameLayout, new HomeFragment()).commit();
        NavigationView.setSelectedItemId(R.id.Nav_Home);

        NavigationView.setOnItemSelectedListener(item -> {
            Fragment fragment = null;

            switch (item.getItemId())
            {
                case R.id.Nav_Home:
                    fragment = new HomeFragment();
                    getSupportFragmentManager().beginTransaction().replace(R.id.FrameLayout, fragment).commit();
                    break;
                case R.id.Nav_Control:
                    fragment = new ControlFragment();
                    getSupportFragmentManager().beginTransaction().replace(R.id.FrameLayout, fragment).commit();
                    break;
                case R.id.Nav_Setting:
                    fragment = new SettingFragment();
                    getSupportFragmentManager().beginTransaction().replace(R.id.FrameLayout, fragment).commit();
                    break;
                case R.id.Nav_Back:
                    I = new Intent(Home.this, CardMenu2.class);
                    startActivity(I);
                    finish();
                    fragment=null;
                    break;

            }
            return true;

        });
    }
}