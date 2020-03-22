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
	private Vector3 _staticHitPos;

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
			Debug.Log(_staticHitPos);
			GameObject go = (GameObject)Resources.Load("Prefabs/BulletHole");
			GameObject bulletHole = Instantiate(go);
			bulletHole.transform.position = _staticHitPos;
			bulletHole.transform.LookAt(transform);
			bulletHole.transform.Translate(Vector3.back * 0.01f);
			Destroy(bulletHole, 5);
		}

		Destroy(gameObject);
	}

	public void AddForce(Vector3 dir)
	{
		this.direction = dir;
		transform.GetComponent<Rigidbody>().AddForce(dir * bulletSpeed, ForceMode.Impulse);
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

	public Vector3 StaticHitPos
	{
		set { _staticHitPos = value; }
	}
}
