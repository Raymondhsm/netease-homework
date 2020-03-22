using UnityEngine;
using UnityEngine.UI;

public class BulletDamageController : MonoBehaviour
{
	public Text damageText;
	public float defaultDamage;

	private Animator _textAnimator;
	private GameObject _parent;
	private Vector3 _position;

    // Start is called before the first frame update
    void Start()
    {
		damageText.text = defaultDamage.ToString();
		_textAnimator = damageText.gameObject.GetComponent<Animator>();
		
    }

    // Update is called once per frame
    void Update()
    {
		if (_textAnimator.GetCurrentAnimatorStateInfo(0).IsTag("End"))
			Destroy(gameObject);

		UpdatePos();
    }

	void UpdatePos()
	{
		Vector3 screenPos = Camera.main.WorldToScreenPoint(_position);
		transform.position = screenPos;
	}

	public float Damage
	{
		set
		{
			damageText.text = value.ToString();
		}
	}

	public GameObject Parent
	{
		set
		{
			transform.parent = value.transform;
			transform.localPosition = Vector3.zero;
		}
	}

	public Vector3 Position
	{
		set
		{
			_position = value;
		}
	}
}
