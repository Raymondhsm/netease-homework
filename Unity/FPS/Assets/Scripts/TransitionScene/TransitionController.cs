using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class TransitionController : MonoBehaviour
{
    public Slider slider;
    private AsyncOperation prog;

    // Start is called before the first frame update
    void Start()
    {
        prog = SceneManager.LoadSceneAsync(2);
        StartCoroutine(LoadingScene());
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    private IEnumerator LoadingScene() 
    {
        prog.allowSceneActivation = false;  
 
        int toProgress = 0;         
        int showProgress = 0;
 
        while (prog.progress < 0.9f) 
        {
            toProgress = (int)(prog.progress * 100);
 
            while ( showProgress < toProgress)
            {
                showProgress++;
                setProgressValue(showProgress);
                
            }
            yield return new WaitForEndOfFrame(); 
        }
        toProgress = 100;                  
 
        while (showProgress < toProgress )
        {
            showProgress++;
            setProgressValue(showProgress);
            yield return new WaitForEndOfFrame(); 
        }
 
        prog.allowSceneActivation = true;  
    }

    private void setProgressValue(int value)
    {
        slider.value = value;
    }
}
