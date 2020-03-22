using UnityEngine;

public class BulletController : MonoBehaviour
{
	public float lifeTime = 5f;
	public float bulletSpeed = 10f;
	[SerializeField]
	private int _bulletDamage = 5;

	private Vector3 direction;
	private float time;
	private GameObject _canvas;

    // Start is called before the first frame update
    void Start()
    {
		time = 0f;
		direction = transform.forward;
		_canvas = GameObject.Find("Canvas");
    }

    // Update is called once per frame
    void Update()
    {
		time += Time.deltaTime;
		if (time > lifeTime) Destroy(gameObject);
		
    }

	private void OnTriggerEnter(Collider other)
	{
		var obj = other.gameObject;
		if(obj.CompareTag("Enemy"))
		{
			GameObject bulletDamage = (GameObject)Resources.Load("Prefabs/BulletDamage");
			GameObject go = Instantiate(bulletDamage);
			BulletDamageController bdc = go.GetComponent<BulletDamageController>();
			bdc.Parent = _canvas;
			bdc.Position = transform.position;
		}
		else if(obj.CompareTag("Terrain"))
		{

		}

		Destroy(gameObject);
	}

	void OnDestroy()
	{

	}

	public int BulletDamage
	{
		set { _bulletDamage = value; }
		get { return _bulletDamage; }
	}

	public Vector3 Direction
	{
		set { direction = value; }
	}


}
